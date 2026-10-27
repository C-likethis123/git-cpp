#include "parsers/CatfileParser.h"
#include "TclapParseScope.h"

CatfileOptions CatfileParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("cat-file", ' ', "0.1");
  TCLAP::UnlabeledValueArg<std::string> typeArg("type", "type of object", true,
                                                "blob", "blob");
  TCLAP::UnlabeledValueArg<std::string> objArg(
      "object", "object hash", true, "e6c0a6d3b2ca0dbb3313843238d7e27f63259d3a",
      "string");
  cmd.ignoreUnmatched(true);
  cmd.add(typeArg);
  cmd.add(objArg);
  cmd.parse(input);
  return {git_object_type_from_string(typeArg.getValue()), objArg.getValue()};
}
