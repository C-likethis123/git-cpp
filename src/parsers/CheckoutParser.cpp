#include "parsers/CheckoutParser.h"
#include "TclapParseScope.h"

CheckoutOptions CheckoutParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("checkout", ' ', "0.2");
  TCLAP::SwitchArg branchArg(
      "b", "branch", "whether to create a new branch from the given point",
      false);
  TCLAP::UnlabeledValueArg<std::string> commitArg(
      "commit", "hash of the commit to checkout to", true, "HEAD",
      "commit hash");
  TCLAP::UnlabeledValueArg<std::string> startPointArg(
      "start-point",
      "If a new branch is created with a  branch name specified, "
      "this argument "
      "will be the starting point of the branch",
      false, "HEAD", "commit hash");
  cmd.ignoreUnmatched(true);
  cmd.add(branchArg);
  cmd.add(commitArg);
  cmd.add(startPointArg);
  cmd.parse(input);
  return {branchArg.getValue(), commitArg.getValue(), startPointArg.getValue(),
          startPointArg.isSet()};
}
