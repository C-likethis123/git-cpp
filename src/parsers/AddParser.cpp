#include "parsers/AddParser.h"
#include <string>
#include <vector>

void AddParser::parse(std::vector<std::string> &args) {
  cmd.reset();
  cmd.parse(args);
}

std::vector<std::string> AddParser::getPaths() const { return pathArg.getValue(); }

AddParser::AddParser()
    : cmd("add", ' ', "0.1"),
      pathArg("path", "paths of the files to add", true, "string") {
  cmd.ignoreUnmatched(true);
  cmd.add(pathArg);
}

AddParser &AddParser::get() {
  static AddParser instance;
  return instance;
}
