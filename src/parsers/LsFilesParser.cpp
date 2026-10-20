#include "parsers/LsFilesParser.h"
#include "tclap/CmdLine.h"

void LsFilesParser::parse(const std::vector<std::string> &args) {
  auto input = args;
  TCLAP::CmdLine cmd("ls-files", ' ', "0.1");
  cmd.ignoreUnmatched(true);
  cmd.parse(input);
}
