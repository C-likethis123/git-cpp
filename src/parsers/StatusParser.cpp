#include "parsers/StatusParser.h"
#include "tclap/CmdLine.h"

void StatusParser::parse(const std::vector<std::string> &args) {
  auto input = args;
  TCLAP::CmdLine cmd("status", ' ', "0.1");
  cmd.ignoreUnmatched(true);
  cmd.parse(input);
}
