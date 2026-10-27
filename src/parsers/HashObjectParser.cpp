#include "parsers/HashObjectParser.h"
#include "TclapParseScope.h"

HashObjectOptions
HashObjectParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("hash-object", ' ', "0.1");
  TCLAP::SwitchArg writeArg("w", "write", "whether to write object to disk",
                            false);
  TCLAP::ValueArg<std::string> typeArg("t", "type", "type of git object", false,
                                       "blob", "type of git object");
  TCLAP::UnlabeledValueArg<std::string> pathArg(
      "path", "file path to create an object from", true, "", "string");
  cmd.ignoreUnmatched(true);
  cmd.add(writeArg);
  cmd.add(typeArg);
  cmd.add(pathArg);
  cmd.parse(input);
  return {writeArg.getValue(), git_object_type_from_string(typeArg.getValue()),
          pathArg.getValue()};
}
