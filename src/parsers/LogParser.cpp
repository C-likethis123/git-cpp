#include "parsers/LogParser.h"
#include "TclapParseScope.h"

LogOptions LogParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("log", ' ', "0.1");
  TCLAP::UnlabeledValueArg<std::string> commitArg(
      "commit", "Display history of given commit", false, "HEAD",
      "commit to start at");
  cmd.ignoreUnmatched(true);
  cmd.add(commitArg);
  cmd.parse(input);
  return {commitArg.getValue()};
}
