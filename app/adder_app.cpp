#include "adder/adder.hpp"
#include "tclap/CmdLine.h"
#include <iostream>

int main(int argc, char** argv){
  if (argc < 2) {
    std::cerr << "Usage: git [command]\n";
  }
  return 0;
}
