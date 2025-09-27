#ifndef TREE_H
#define TREE_H

#include "object.h"
#include "repository.h"
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;
class GitTree : public GitObject {
public:
  GitTree(const std::string &data = std::string(""));
  static GitTree read(GitRepository &repo, const std::string &hash);

  void deserialise(
      const std::string &data) override; // convert string format to data object
  std::string
  serialise(GitRepository &repo) override; // convert this to a string format
  std::string print_matching_files(
      GitRepository &repo,
      const std::string &filePathPattern); // print tree entries that
                                           // match the given file path

  // maybe i need something like a git tree entry?
  std::optional<std::string> find(std::string &file_name);
  static void instantiate_tree(
      GitTree &treeToInstantiate, GitTree &curr_tree,
      const fs::path
          &path); // instantiate a tree object in the working directory
  static void instantiate_tree(
      GitTree &treeToInstantiate,
      const fs::path
          &path); // instantiate a tree object in the working directory
  void init();
  // move this to protected later
  std::vector<std::string> pathNames;
  std::unordered_map<std::string, std::tuple<int, std::string>> fileEntries;
};

#endif // TREE_H
