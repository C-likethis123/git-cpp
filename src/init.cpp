#include <iostream>
#include "commands/init.h"

#include "tclap/CmdLine.h"
#include "repository.h"

void init(std::vector<std::string> &args) {
    TCLAP::CmdLine cmd("init", ' ', "0.9");

    // defines arguments
    TCLAP::ValueArg<std::string> pathArg("p","path","Path to create the git repo",false,".","string");
    cmd.ignoreUnmatched(true);
    cmd.add(pathArg);
    cmd.parse(args);
    std::string path = pathArg.getValue();

    // process args
    GitRepository repo(path, false);
    std::string worktree = repo.create(false);
    std::cout << "Initialised Git repo at " << worktree << "\n";
}