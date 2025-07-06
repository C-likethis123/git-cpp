#include "index.h"
#include "index_entry.h"
#include "repository.h"
#include "util.h"
#include <cmath>
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

GitIndex::GitIndex(uint32_t fileVersion, std::vector<GitIndexEntry> entries)
    : version_(fileVersion), entries_(std::move(entries)){};

GitIndex GitIndex::read(GitRepository &repo) {
  fs::path file_path = repo.repo_path("index");
  std::string fileContents = read_file(file_path);
  uint32_t fileVersion = read_uint32_from_bytes(fileContents, 4);
  uint32_t fileEntryCount = read_uint32_from_bytes(fileContents, 8);
  size_t ptr = 0;
  fileContents = fileContents.substr(12);
  std::vector<GitIndexEntry> indexEntries;
  for (uint32_t i = 0; i < fileEntryCount; ++i) {
    uint32_t ctime_sec = read_uint32_from_bytes(fileContents, ptr);
    uint32_t ctime_nanosec = read_uint32_from_bytes(fileContents, ptr + 4);
    uint32_t mtime_sec = read_uint32_from_bytes(fileContents, ptr + 8);
    uint32_t mtime_nanosec = read_uint32_from_bytes(fileContents, ptr + 12);
    uint32_t dev = read_uint32_from_bytes(fileContents, ptr + 16);
    uint32_t ino = read_uint32_from_bytes(fileContents, ptr + 20);
    uint32_t mode = read_uint32_from_bytes(fileContents, ptr + 24);
    uint8_t object_type = (mode << 16) >> 12; // bits 31..28
    uint8_t unused = (mode >> 20) >> 9;       // bits 27..25
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
    if (file_name_length < 0xFFF) {
      file_name = fileContents.substr(ptr + 62, file_name_length);
      ptr = ceil((ptr + 62 + file_name_length + 1) / 8.0) * 8;

    } else {
      size_t idx = fileContents.find('\x00', ptr + 64);
      file_name = fileContents.substr(ptr + 64, idx - ptr - 64);
      ptr = ceil((idx + 1) / 8.0) * 8;
    }
    indexEntries.emplace_back(ctime_sec, ctime_nanosec, mtime_sec,
                              mtime_nanosec, dev, ino, mode, object_type,
                              unused, permissions, uid, gid, file_size, sha1,
                              flag_assume_valid, flag_extended, flag_stage,
                              file_name_length, file_name);
  }

  return GitIndex(fileVersion, indexEntries);
}

void GitIndex::print_matching_patterns(GitRepository &repo,
                                       const std::string &path) {
  for (auto &entry : entries_) {
    std::string file_name = entry.file_name();
    fs::path path_in_repo = repo.worktree_path(file_name);
    fs::path prefix_to_remove = repo.worktree_path(path);
    // TODO: fix bug where the file name is too long to handle
    if (file_name.rfind(path, 0) == 0) {
      std::cout << fs::relative(path_in_repo, prefix_to_remove).string()
                << std::endl;
    }
  }
}
