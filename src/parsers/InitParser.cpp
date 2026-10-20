#include "parsers/InitParser.h"
#include "tclap/CmdLine.h"

InitOptions InitParser::parse(const std::vector<std::string> &args) {
  auto input = args;
  TCLAP::CmdLine cmd("init", ' ', "0.1");
  TCLAP::ValueArg<std::string> pathArg(
      "p", "path", "Path to create the git repo", false, ".", "string");
  cmd.ignoreUnmatched(true);
  cmd.add(pathArg);
  cmd.parse(input);
  return {pathArg.getValue()};
}
