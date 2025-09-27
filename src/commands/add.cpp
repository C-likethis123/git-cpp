#include "commands/add.h"
#include "parsers/AddParser.h"
#include <iostream>

namespace commands {
void add(std::vector<std::string> &args) {
  AddParser &parser = AddParser::get();
  parser.parse(args);
  std::string path = parser.getPath();
  std::cout << "add" << path << std::endl;
}
} // namespace commands
