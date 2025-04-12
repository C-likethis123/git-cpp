#include "tree.h"
#include "repository.h"
#include "util.h"
#include <iostream>
#include <sstream>

GitTree::GitTree(const std::string &data) : GitObject("tree") {
  this->deserialise(data);
};

std::string GitTree::serialise(GitRepository &repo) {
  std::stringstream ss;
  for (const auto &path : this->pathNames) {
    auto &[mode, sha] = this->fileEntries[path];
    ss << mode << " " << path << '\0' << hexToBinary(sha);
  }
  return ss.str();
}

void GitTree::deserialise(const std::string &data) {
  int curr = 0;
  while (curr < data.size()) {
    int space = data.find(' ', curr);
    int mode = std::stoi(data.substr(curr, space - curr));
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

std::string GitTree::print_matching_files(const std::string &filePath) {
  std::stringstream ss;
  for (const auto &path : this->pathNames) {
    // TODO: implement better file path matching
    if (filePath.empty() || path == filePath) {
      auto &[mode, sha] = this->fileEntries[path];
      ss << mode << " " << sha << " " << path << "\n";
    }
  }
  return ss.str();
}
