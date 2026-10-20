#include "parsers/AddParser.h"
#include "TclapParseScope.h"

AddOptions AddParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("add", ' ', "0.1");
  TCLAP::UnlabeledMultiArg<std::string> pathArg(
      "path", "paths of the files to add", true, "string");
  cmd.ignoreUnmatched(true);
  cmd.add(pathArg);
  cmd.parse(input);
  return {pathArg.getValue()};
}
