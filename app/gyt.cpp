#include "commands/cat-file.h"
#include "commands/checkout.h"
#include "commands/hash-object.h"
#include "commands/init.h"
#include "commands/log.h"
#include "commands/ls-tree.h"
#include "commands/show-ref.h"
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
    commands::init(args);
  } else if (command == "cat-file") {
    commands::catfile(args);
  } else if (command == "hash-object") {
    commands::hashobject(args);
  } else if (command == "log") {
    commands::log(args);
  } else if (command == "ls-tree") {
    commands::lstree(args);
  } else if (command == "checkout") {
    commands::checkout(args);
  } else if (command == "show-ref") {
    commands::showref(args);
  } else {
    std::cerr << "Unknown command: " << command << "\n";
    return -1;
  }
  return 0;
}
