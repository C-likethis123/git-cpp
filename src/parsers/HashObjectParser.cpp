#include "parsers/HashObjectParser.h"
#include <string>
#include <vector>

void HashObjectParser::parse(std::vector<std::string> &args) {
  cmd.reset();
  cmd.parse(args);
}

const std::string &HashObjectParser::getPath() const {
  return pathArg.getValue();
}
bool HashObjectParser::isWrite() const { return writeArg.getValue(); }
const std::string &HashObjectParser::getType() const {
  return typeArg.getValue();
}

HashObjectParser &HashObjectParser::get() {
  static HashObjectParser instance;
  return instance;
}

HashObjectParser::HashObjectParser()
    : cmd("hash-object", ' ', "0.1"),
      writeArg("w", "write", "whether to write object to disk", false),
      typeArg("t", "type", "type of git object", false, "blob",
              "type of git object"),
      pathArg("path", "file path to create an object from", true, "",
              "string") {
  cmd.ignoreUnmatched(true);
  cmd.add(writeArg);
  cmd.add(typeArg);
  cmd.add(pathArg);
}
