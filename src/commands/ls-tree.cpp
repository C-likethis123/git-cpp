#include "commands/ls-tree.h"
#include <iostream>
#include <string>

#include "object.h"
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
  GitObject *obj = GitObject::read(repo, treeHash);
  // TODO: why did I use dynamic cast instead of static_cast here?
  GitTree *tree = dynamic_cast<GitTree *>(obj);
  if (obj) {
    std::cout << tree->print_matching_files(repo, filePathPattern);
  }
}
} // namespace commands
