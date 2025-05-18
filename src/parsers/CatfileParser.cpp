#include "parsers/CatfileParser.h"
#include <string>
#include <vector>

void CatfileParser::parse(std::vector<std::string> &args) {
  cmd.reset();
  cmd.parse(args);
}

std::string CatfileParser::getType() const { return typeArg.getValue(); }
std::string CatfileParser::getHash() const { return objArg.getValue(); }

CatfileParser &CatfileParser::get() {
  static CatfileParser instance;
  return instance;
}

CatfileParser::CatfileParser()
    : cmd("cat-file", ' ', "0.1"),
      typeArg("type", "type of object", true, "blob", "blob"),
      objArg("object", "object hash", true,
             "e6c0a6d3b2ca0dbb3313843238d7e27f63259d3a", "string") {
  cmd.ignoreUnmatched(true);
  cmd.add(typeArg);
  cmd.add(objArg);
}
