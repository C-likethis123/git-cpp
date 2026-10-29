#include "commands/hash-object.h"
#include <iostream>

#include "object.h"
#include "parsers/HashObjectParser.h"
#include "util.h"

namespace commands {
void hashobject(std::vector<std::string> &args) {
  const auto options = HashObjectParser::parse(args);
  bool write = options.write;
  const auto type = options.type;
  const std::string &path = options.path;

  std::string fileContents = read_file(fs::path(path));
  // write to disk optionally, print hash
  std::string hash = GitObject::write(type, fileContents, write);
  std::cout << hash << "\n";
}
} // namespace commands
