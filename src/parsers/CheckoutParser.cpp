#include "parsers/CheckoutParser.h"
#include <string>
#include <vector>

void CheckoutParser::parse(std::vector<std::string> &args) {
  cmd.reset();
  cmd.parse(args);
}

bool CheckoutParser::isCommitSet() const { return commitArg.isSet(); }
std::string CheckoutParser::getCommit() { return commitArg.getValue(); }

bool CheckoutParser::isCreateNewBranch() const { return branchArg.getValue(); }

std::string CheckoutParser::getStartPoint() { return startPointArg.getValue(); }
bool CheckoutParser::isStartPointSet() const { return startPointArg.isSet(); }

CheckoutParser &CheckoutParser::get() {
  static CheckoutParser instance;
  return instance;
}

CheckoutParser::CheckoutParser()
    : cmd("checkout", ' ', "0.2"),
      branchArg("b", "branch",
                "whether to create a new branch from the given point", false),
      commitArg("commit", "hash of the commit to checkout to", true, "HEAD",
                "commit hash"),
      startPointArg("start-point",
                    "If a new branch is created with a  branch name specified, "
                    "this argument "
                    "will be the starting point of the branch",
                    false, "HEAD", "commit hash") {
  cmd.ignoreUnmatched(true);
  cmd.add(branchArg);
  cmd.add(commitArg);
  cmd.add(startPointArg);
}
