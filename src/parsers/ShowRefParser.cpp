#include "parsers/ShowRefParser.h"
#include "tclap/CmdLine.h"

void ShowRefParser::parse(const std::vector<std::string> &args) {
  auto input = args;
  TCLAP::CmdLine cmd("show-ref", ' ', "0.1");
  cmd.parse(input);
}
