#include "TclapParseScope.h"
#include "parsers/CheckIgnoreParser.h"
#include "tclap/CmdLine.h"
#include "tclap/UnlabeledMultiArg.h"

CheckIgnoreOptions
CheckIgnoreParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("check-ignore", ' ', "0.1");
  TCLAP::UnlabeledMultiArg<std::string> filePathPatternsArg(
      "path", "patterns that matches file names", false, "", "string");
  cmd.ignoreUnmatched(true);
  cmd.add(filePathPatternsArg);
  cmd.parse(input);
  return {filePathPatternsArg.getValue()};
}
