#include "tree.h"
#include "blob.h"
#include "commit.h"
#include "object.h"
#include "repo_util.h"
#include "repository.h"
#include "util.h"
#include "wildmatch/wildmatch.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

GitTree::GitTree(const std::string &data) : GitObject(GitObjectType::Tree) {
  this->deserialise(data);
};

GitTree GitTree::read(const std::string &hash) {
  const std::string &sha = GitObject::find(hash);
  auto [fmt, payload] = read_git_object_data(sha);
  if (fmt == "tree") {
    return GitTree(payload);
  } else if (fmt == "commit") {
    GitCommit commitObj = GitCommit::read(hash);
    std::string treeHash = commitObj.get_tree();
    return read(treeHash);
  } else {
    throw std::runtime_error("Invalid tree object: " + hash);
  }
}

std::string GitTree::serialise(GitRepository &repo) {
  std::stringstream ss;
  auto paths = pathNames;
  // Git compares directory names as if they end in '/'.
  std::sort(paths.begin(), paths.end(), [this](const auto &a, const auto &b) {
    const auto key = [this](const auto &path) {
      return path + (std::get<0>(fileEntries.at(path)) == 0040000 ? "/" : "");
    };
    const auto left = key(a);
    const auto right = key(b);
    return std::lexicographical_compare(
        left.begin(), left.end(), right.begin(), right.end(),
        [](unsigned char x, unsigned char y) { return x < y; });
  });
  for (const auto &path : paths) {
    const auto &[mode, sha] = this->fileEntries.at(path);
    ss << std::oct << mode << " " << path << '\0' << hexToBinary(sha);
  }
  return ss.str();
}

void GitTree::deserialise(const std::string &data) {
  int curr = 0;
  while (curr < data.size()) {
    int space = data.find(' ', curr);
    int mode = std::stoi(data.substr(curr, space - curr), nullptr, 8);
    curr = space + 1;
    space = data.find('\0', curr);
    std::string path = data.substr(curr, space - curr);
    curr = space + 1;
    std::string sha = binaryToHex(data.substr(curr, 20));
    curr += 20;
    this->fileEntries[path] = {mode, sha};
    pathNames.push_back(path);
  }
  // maintain sort order via path names to ensure
  // consistent tree objects are generated each time
  std::sort(pathNames.begin(), pathNames.end());
}

void GitTree::print_matching_files(
    GitRepository &repo, const std::vector<std::string> &filePathPatterns) {
  auto findDot =
      std::find(filePathPatterns.begin(), filePathPatterns.end(), ".");
  bool isPrintEveryFile =
      findDot != filePathPatterns.end() || filePathPatterns.empty();
  for (const auto &path : this->pathNames) {
    bool isFileMatch = isPrintEveryFile;
    // file path match happens if a globbing sequence matches OR
    // if it's a full match.
    if (!isFileMatch) {
      for (const auto &filePath : filePathPatterns) {
        if (wild::match(filePath, path)) {
          isFileMatch = true;
          break;
        }
      }
    }

    if (isFileMatch) {
      auto &[mode, sha] = this->fileEntries[path];
      std::string type = get_file_type(mode);
      std::cout << std::setfill('0') << std::setw(6) << std::oct << mode << " "
                << type << " " << sha << '\t' << path << "\n";
    }
  }
}

/**

take a path, and 2 tree objects
go through the current tree:
- if not found in the tree, just create the file or the directory.
- if found in the tree: check the sha + file mode
        - if what's currently in the directory is a file, remove the file.
        - if what's currently in the directory is a directory, remove the
directory if the new sha points to a file.
        - if both are folders, don't remove, recursively call (can call this
function)
  - if the file mode is different, change the file mode.
*/
void GitTree::instantiate_tree(GitTree &treeToInstantiate, GitTree &curr_tree,
                               const fs::path &path) {
  GitRepository repo = GitRepository::find();
  for (const auto &filePath : treeToInstantiate.pathNames) {
    auto &[mode, sha] = treeToInstantiate.fileEntries[filePath];
    const auto new_file_type = get_file_type(mode);
    const auto new_file_path = path / filePath;
    // attempt to find it in the current tree
    auto it = curr_tree.fileEntries.find(filePath);
    if (it != curr_tree.fileEntries.end()) {
      auto &[curr_mode, curr_sha] = it->second;
      if (curr_sha != sha) {
        const auto tree_file_type = get_file_type(curr_mode);
        if (tree_file_type == "blob") {
          // assume that the directory contents match the tree contents here
          fs::remove(new_file_path);
          GitBlob obj = GitBlob::read(repo, sha);
          create_file(new_file_path, obj.serialise(repo));
        } else if (tree_file_type == "tree" && new_file_type == "blob") {
          fs::remove_all(new_file_path);
          GitBlob obj = GitBlob::read(repo, sha);
          create_file(new_file_path, obj.serialise(repo));
        } else {
          GitTree subTreeToInstantiate = GitTree::read(repo, sha);
          GitTree curr_subTree = GitTree::read(repo, curr_sha);
          instantiate_tree(subTreeToInstantiate, curr_subTree, new_file_path);
        }
      }
      // assumption here: the file is created, so change the file mode
      if (mode != curr_mode) {
        fs::permissions(new_file_path, get_unix_permissions(mode));
      }
    } else {
      // not found in the current tree, create it
      if (new_file_type == "blob") {
        GitBlob obj = GitBlob::read(repo, sha);
        create_file(new_file_path, obj.serialise(repo));
      } else {
        fs::create_directories(new_file_path);
        GitTree subTreeToInstantiate = GitTree::read(repo, sha);
        instantiate_tree(subTreeToInstantiate, new_file_path);
      }
      // assumption here: the file is created, so change the file mode
      fs::permissions(new_file_path, get_unix_permissions(mode));
    }
  }
  // remove items that are not in the tree that I am instantiating
  for (const auto &filePath : curr_tree.pathNames) {
    auto it = treeToInstantiate.fileEntries.find(filePath);
    if (it == treeToInstantiate.fileEntries.end()) {
      auto &[mode, sha] = curr_tree.fileEntries[filePath];
      const auto curr_file_type = get_file_type(mode);
      const auto curr_file_path = path / filePath;
      if (curr_file_type == "blob") {
        fs::remove(curr_file_path);
      } else {
        fs::remove_all(curr_file_path);
      }
    }
  }
}

/**
Instantiates a tree that is not instantiated in the current working directory
*/
void GitTree::instantiate_tree(GitTree &treeToInstantiate,
                               const fs::path &path) {
  GitRepository repo = GitRepository::find();
  for (const auto &filePath : treeToInstantiate.pathNames) {
    auto &[mode, sha] = treeToInstantiate.fileEntries[filePath];
    const auto new_file_type = get_file_type(mode);
    const auto new_file_path = path / filePath;
    if (new_file_type == "blob") {
      GitBlob obj = GitBlob::read(repo, sha);
      create_file(new_file_path, obj.serialise(repo));
    } else {
      fs::create_directories(new_file_path);
      GitTree subTreeToInstantiate = GitTree::read(repo, sha);
      instantiate_tree(subTreeToInstantiate, new_file_path);
    }

    fs::permissions(new_file_path, get_unix_permissions(mode));
  }
}
