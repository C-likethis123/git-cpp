#include "commands/init.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char** argv){
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [command]\n";
    return -1;
  }
  std::string command = argv[1];
  std::vector<std::string> args;
  for (int i = 0; i < argc; i++) {
    args.push_back(argv[i]);
  }
  if (command == "init") {
    init(args);
  }
  return 0;
}
