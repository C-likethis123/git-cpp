#include <iostream>
#include "tclap/CmdLine.h"
#include "commands/init.h"
void init(std::vector<std::string> &args) {
    TCLAP::CmdLine cmd("init", ' ', "0.9");

    // defines arguments
    TCLAP::ValueArg<std::string> path("p","path","Path to create the git repo",false,".","string");
    cmd.ignoreUnmatched(true);
    cmd.add(path);
    cmd.parse(args);
    std::string pathValue = path.getValue();
    std::cout << "Path: " << pathValue << "\n";
}