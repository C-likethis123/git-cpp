#include "adder/adder.hpp"
#include "commands/init.h"
#include <iostream>

int main(int argc, char** argv){
  if (argc < 2) {
    std::cerr << "Usage: git [command]\n";
  }
  std::string command = argv[1];
  if (command == "init") {
    init(argv);
  }
  return 0;
}
