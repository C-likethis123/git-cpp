#include "index.h"
#include "commit.h"
#include "index_entry.h"
#include "object.h"
#include "repository.h"
#include "tree.h"
#include "util.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

// TODO: improve this
// Helper function to build a map of file paths to tree entries
void build_file_map(const GitTree &tree, const std::string &prefix,
                    std::unordered_map<std::string, std::string> &file_map,
                    GitRepository &repo) {
  for (const auto &path : tree.pathNames) {
    std::string full_path = prefix.empty() ? path : prefix + "/" + path;
    auto &[mode, sha] = tree.fileEntries.at(path);

    if (mode == 0100644 || mode == 0100755) {
      // It's a file (100644 = regular file, 100755 = executable)
      file_map[full_path] = sha;
    } else if (mode == 0040000) {
      // It's a directory (040000), recurse into it
      GitTree sub_tree = GitTree::read(repo, sha);
      build_file_map(sub_tree, full_path, file_map, repo);
    }
  }
}

GitIndex::GitIndex(uint32_t fileVersion,
                   std::unordered_map<std::string, GitIndexEntry> entries)
    : version_(fileVersion), entries_(std::move(entries)){};

GitIndex GitIndex::read(GitRepository &repo) {
  fs::path file_path = repo.repo_path("index");
  std::string fileContents = read_file(file_path);
  uint32_t fileVersion = read_uint32_from_bytes(fileContents, 4);
  uint32_t fileEntryCount = read_uint32_from_bytes(fileContents, 8);
  size_t ptr = 12;
  std::unordered_map<std::string, GitIndexEntry> indexEntries;
  for (uint32_t i = 0; i < fileEntryCount; ++i) {
    uint32_t ctime_sec = read_uint32_from_bytes(fileContents, ptr);
    uint32_t ctime_nanosec = read_uint32_from_bytes(fileContents, ptr + 4);
    uint32_t mtime_sec = read_uint32_from_bytes(fileContents, ptr + 8);
    uint32_t mtime_nanosec = read_uint32_from_bytes(fileContents, ptr + 12);
    uint32_t dev = read_uint32_from_bytes(fileContents, ptr + 16);
    uint32_t ino = read_uint32_from_bytes(fileContents, ptr + 20);
    uint32_t mode = read_uint32_from_bytes(fileContents, ptr + 24);
    uint8_t object_type = (mode << 16) >> 12; // bits 31..28
    uint8_t unused = mode >> 29;              // bits 27..25
    uint16_t permissions = mode & 0x1FF;      // bits 24..16
    uint32_t uid = read_uint32_from_bytes(fileContents, ptr + 28);
    uint32_t gid = read_uint32_from_bytes(fileContents, ptr + 32);
    uint32_t file_size = read_uint32_from_bytes(fileContents, ptr + 36);
    std::string sha1 = binaryToHex(fileContents.substr(ptr + 40, 20));
    uint32_t file_name_and_status =
        read_uint32_from_bytes(fileContents, ptr + 60) >> 16;
    bool flag_assume_valid = (file_name_and_status & 0x8000) != 0;
    bool flag_extended = (file_name_and_status & 0x4000) != 0;
    uint8_t flag_stage = (file_name_and_status & 0x3000);
    uint32_t file_name_length = file_name_and_status & 0xFFF;
    std::string file_name{};
    // had to subtract 12 from the pointer calculation because the file
    // contents are read from the file starting at 12 bytes in.
    if (file_name_length < 0xFFF) {
      file_name = fileContents.substr(ptr + 62, file_name_length);
      ptr = ceil((ptr + 50 + file_name_length + 1) / 8.0) * 8 + 12;

    } else {
      size_t idx = fileContents.find('\x00', ptr + 64);
      file_name = fileContents.substr(ptr + 64, idx - ptr - 64);
      ptr = ceil((idx - 12 + 1) / 8.0) * 8 + 12;
    }
    indexEntries.try_emplace(file_name, ctime_sec, ctime_nanosec, mtime_sec,
                             mtime_nanosec, dev, ino, mode, object_type, unused,
                             permissions, uid, gid, file_size, sha1,
                             flag_assume_valid, flag_extended, flag_stage,
                             file_name_length, file_name);
  }

  return GitIndex(fileVersion, indexEntries);
}

void GitIndex::print_matching_patterns(GitRepository &repo,
                                       const std::string &path) {
  for (auto &[file_name, entry] : entries_) {
    fs::path path_in_repo = repo.worktree_path(file_name);
    fs::path prefix_to_remove = repo.worktree_path(path);
    // TODO: fix bug where the file name is too long to handle
    if (file_name.rfind(path, 0) == 0) {
      std::cout << fs::relative(path_in_repo, prefix_to_remove).string()
                << std::endl;
    }
  }
}

/**
TODO: break this up
Scans for staged changes and unstaged changes.

Staged changes are compared between the current index and the HEAD commit.
Unstaged changes are compared between the current index and the current working
directory.

Gitignore is respected, but it's not perfect.
*/
void GitIndex::scan_status(GitRepository &repo) {
  std::vector<std::string> modified;
  std::vector<std::string> untracked;
  std::vector<std::string> deleted;

  std::cout << "Changes not staged for commit:" << std::endl;
  for (auto it = fs::recursive_directory_iterator(repo.worktree_path("."));
       it != fs::recursive_directory_iterator(); ++it) {
    const auto &entry = *it;
    if (repo.is_ignored(entry.path())) {
      if (entry.is_directory()) {
        it.disable_recursion_pending();
      }
      continue;
    }
    if (entry.is_directory()) {
      continue;
    }
    // find the relative path, eg "src/index.cpp"
    fs::path relative_path =
        fs::relative(entry.path(), repo.worktree_path("."));
    std::string relative_path_str = relative_path.string();
    bool found_in_index = false;
    auto index_entry = entries_.find(relative_path_str);
    if (index_entry != entries_.end()) {
      found_in_index = true;
      // check if modified
      // TODO: if an entire folder is modified or has changes, ignore the
      // rest.
      auto file_time = fs::last_write_time(entry.path());
      auto file_time_sec = std::chrono::duration_cast<std::chrono::seconds>(
                               file_time.time_since_epoch())
                               .count();
      auto file_size = fs::file_size(entry.path());

      if (file_time_sec != index_entry->second.mtime_sec() ||
          file_size != index_entry->second.file_size()) {
        std::string file_sha1 =
            GitObject::write(repo, "blob", read_file(entry.path()), false);
        modified.emplace_back(relative_path_str);
      }
    }

    if (!found_in_index) {
      untracked.emplace_back(relative_path_str);
    }
  }

  for (const auto &file : modified) {
    std::cout << "modified: " << file << std::endl;
  }
  for (const auto &file : untracked) {
    std::cout << "untracked: " << file << std::endl;
  }
  for (const auto &file : deleted) {
    std::cout << "deleted: " << file << std::endl;
  }

  std::vector<std::string> staged_modifications;
  std::vector<std::string> staged_additions;
  std::vector<std::string> staged_deletions;

  // Get the HEAD commit and its tree
  GitCommit head = GitCommit::read(repo, "HEAD");
  GitTree treeObj = GitTree::read(repo, head.get_tree());

  // Create a map of file paths to SHA1 hashes for efficient lookup
  std::unordered_map<std::string, std::string> head_files;
  build_file_map(treeObj, "", head_files, repo);

  // Compare index entries with HEAD tree
  for (const auto &[file_path, index_entry] : entries_) {
    if (head_files.find(file_path) != head_files.end()) {
      // File exists in HEAD, check if it's modified
      const std::string &head_sha = head_files[file_path];
      if (index_entry.sha1() != head_sha) {
        staged_modifications.emplace_back(file_path);
      }
    } else {
      // File doesn't exist in HEAD, it's a staged addition
      staged_additions.emplace_back(file_path);
    }
  }

  // Check for staged deletions (files in HEAD but not in index)
  for (const auto &head_file : head_files) {
    bool found_in_index = false;
    if (entries_.find(head_file.first) == entries_.end()) {
      staged_deletions.emplace_back(head_file.first);
    }
  }

  // Print staged changes
  std::cout << "Changes to be committed:" << std::endl;
  for (const auto &file : staged_modifications) {
    std::cout << "modified: " << file << std::endl;
  }
  for (const auto &file : staged_additions) {
    std::cout << "new file: " << file << std::endl;
  }
  for (const auto &file : staged_deletions) {
    std::cout << "deleted: " << file << std::endl;
  }
}

void GitIndex::add_file(const std::string &path, GitRepository &repo) {
  entries_.erase(path);

  entries_.emplace(path, GitIndexEntry::create_index_entry(path, repo));
}

void GitIndex::save(GitRepository &repo) {
  std::stringstream filestream;

  // Write header (12 bytes)
  filestream.write("DIRC", 4);                        // Signature
  write_uint32_to_bytes(filestream, version_);        // Version
  write_uint32_to_bytes(filestream, entries_.size()); // Entry count

  // Create a vector of pairs for sorting
  std::vector<std::pair<std::string, GitIndexEntry>> sorted_entries(
      entries_.begin(), entries_.end());
  std::sort(sorted_entries.begin(), sorted_entries.end(),
            [](const auto &a, const auto &b) {
              return a.first < b.first; // Sort by file name
            });

  for (const auto &[file_name, entry] : sorted_entries) {
    entry.save(repo, filestream);
  }

  const std::string index_sha = sha1_hexdigest(filestream.str());
  filestream.write(hexToBinary(index_sha).c_str(), 20);
  fs::path index_path = repo.repo_path("index_test");
  create_file(index_path, filestream.str());
}
