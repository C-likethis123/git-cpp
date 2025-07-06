#include "commands/ls-tree.h"
#include <iostream>
#include <string>

#include "repository.h"
#include "tclap/CmdLine.h"
#include "tree.h"

namespace commands {
void lstree(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("ls-tree", ' ', "0.1");

  // defines arguments
  TCLAP::UnlabeledValueArg<std::string> treeObjectArg(
      "tree-ish", "commit ID of tree object", true, "HEAD", "string");
  TCLAP::UnlabeledValueArg<std::string> filePathPatternArg(
      "path", "patterns that matches file names", false, "", "string");

  cmd.ignoreUnmatched(true);
  cmd.add(treeObjectArg);
  cmd.add(filePathPatternArg);
  cmd.parse(args);
  std::string &treeHash = treeObjectArg.getValue();
  std::string &filePathPattern = filePathPatternArg.getValue();

  // process args
  GitRepository repo = GitRepository::find();
  GitTree tree = GitTree::read(repo, treeHash);
  std::cout << tree.print_matching_files(repo, filePathPattern);
}
} // namespace commands
