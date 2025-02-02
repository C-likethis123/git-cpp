#include <iostream>
#include "commands/hash-object.h"

#include "tclap/CmdLine.h"
#include "repository.h"
#include "object.h"

/**
Basic tests:
*/
void hashobject(std::vector<std::string> &args) {
    TCLAP::CmdLine cmd("hash-object", ' ', "0.1");

    // defines arguments
    TCLAP::SwitchArg writeArg("w","write","whether to write object to disk",false);
    TCLAP::UnlabeledValueArg<std::string> typeArg("type","type of object",true,"blob","blob");
    TCLAP::UnlabeledValueArg<std::string> objArg("object","object hash",true,"e6c0a6d3b2ca0dbb3313843238d7e27f63259d3a","string");

    cmd.add(writeArg);
    cmd.add(typeArg);
    cmd.add(objArg);
    cmd.parse(args);
    bool write = writeArg.getValue();
    std::string& type = typeArg.getValue();
    std::string& hash = objArg.getValue();

    // process args
    try {
        std::optional<GitRepository> repo = GitRepository::find();
        if (repo){
            // read file
            // compute hash
            // write to disk or print hash

            //maybe I can use gitblob?
        }
    } catch (std::runtime_error& err) {
        std::cerr << err.what() << "\n";
    }
    
    
}