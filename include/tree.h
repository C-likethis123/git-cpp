#ifndef TREE_H
#define TREE_H

#include "object.h"
#include <boost/filesystem.hpp>
#include <string>
#include <unordered_map>

namespace fs = boost::filesystem;
class GitTree : public GitObject {
public:
  GitTree(const std::string &data = std::string(""));

  void deserialise(
      const std::string &data) override; // convert string format to data object
  std::string
  serialise(GitRepository &repo) override; // convert this to a string format
  std::string print_matching_files(
      GitRepository &repo,
      const std::string &filePathPattern); // print tree entries that
                                           // match the given file path
  static void instantiate_tree(
      GitTree *treeToInstantiate, GitTree *curr_tree,
      const fs::path
          &path); // instantiate a tree object in the working directory
  void init();

protected:
  std::vector<std::string> pathNames;
  std::unordered_map<std::string, std::tuple<int, std::string>> fileEntries;
};

#endif // TREE_H
