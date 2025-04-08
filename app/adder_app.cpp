#include "commands/cat-file.h"
#include "commands/hash-object.h"
#include "commands/init.h"
#include "commands/log.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [command]\n";
    return -1;
  }
  std::string command = argv[1];
  std::vector<std::string> args;
  for (int i = 1; i < argc; i++) {
    args.push_back(argv[i]);
  }
  if (command == "init") {
    init(args);
  } else if (command == "cat-file") {
    catfile(args);
  } else if (command == "hash-object") {
    hashobject(args);
  } else if (command == "log") {
    log(args);
  } else {
    std::cerr << "Unknown command: " << command << "\n";
    return -1;
  }
  return 0;
}
