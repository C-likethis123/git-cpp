#include "commands/ls-tree.h"
#include <iostream>
#include <string>

#include "parsers/LsTreeParser.h"
#include "repository.h"
#include "tree.h"

namespace commands {
void lstree(std::vector<std::string> &args) {
  const auto options = LsTreeParser::parse(args);

  const std::string &treeHash = options.treeObject;
  const std::vector<std::string> &filePathPatterns = options.filePathPatterns;

  GitRepository repo = GitRepository::find();
  GitTree tree = GitTree::read(repo, treeHash);
  tree.print_matching_files(repo, filePathPatterns);
}
} // namespace commands
