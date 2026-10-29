#include "parsers/MergeParser.h"
#include "TclapParseScope.h"

MergeOptions MergeParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("merge", ' ', "0.1");
  cmd.setExceptionHandling(false);
  TCLAP::UnlabeledValueArg<std::string> commitArg(
      "commit", "Branch or commit to merge into the current branch", true, "",
      "commit");
  cmd.add(commitArg);
  cmd.parse(input);
  return {commitArg.getValue()};
}
