#include <iostream>
#include "commands/catfile.h"

#include "tclap/CmdLine.h"
#include "repository.h"

void catfile(std::vector<std::string> &args) {
    TCLAP::CmdLine cmd("cat-file", ' ', "0.1");

    // defines arguments
    TCLAP::UnlabeledValueArg<std::string> typeArg("t","type","type of object: [blob, commit, tag, tree]",true,".","string");
    TCLAP::UnlabeledValueArg<std::string> objArg("o","object","object name",true,".","string");

    cmd.add(typeArg);
    cmd.add(objArg);
    cmd.parse(args);
    std::string& type = typeArg.getValue();
    std::string& objArg = objArg.getValue();

    // process args
    GitRepository& repo = repo_find();
    GitObject &obj = GitObject::read(repo, GitObject::find(repo, objArg, type));
}