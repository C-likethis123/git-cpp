#include "commands/ls-tree.h"
#include <string>

#include "parsers/LsTreeParser.h"
#include "tree.h"

namespace commands {
void lstree(std::vector<std::string> &args) {
  const auto options = LsTreeParser::parse(args);

  const std::string &treeHash = options.treeObject;
  const std::vector<std::string> &filePathPatterns = options.filePathPatterns;

  GitTree tree = GitTree::read(treeHash);
  tree.print_matching_files(filePathPatterns);
}
} // namespace commands
