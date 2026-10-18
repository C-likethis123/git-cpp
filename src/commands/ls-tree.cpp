#include "commands/ls-tree.h"
#include <iostream>
#include <string>

#include "repository.h"
#include "tclap/CmdLine.h"
#include "tree.h"
#include "parsers/LsTreeParser.h"

namespace commands {
void lstree(std::vector<std::string> &args) {
  LsTreeParser& parser = LsTreeParser::get();
  parser.parse(args);

  const std::string &treeHash = parser.getTreeObject();
  const std::vector<std::string> &filePathPatterns = parser.getFilePathPatterns();

  GitRepository repo = GitRepository::find();
  GitTree tree = GitTree::read(repo, treeHash);
  std::cout << tree.print_matching_files(repo, filePathPatterns);
}
} // namespace commands
