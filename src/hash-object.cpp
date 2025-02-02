#include <iostream>
#include "commands/hash-object.h"

#include "tclap/CmdLine.h"
#include "repository.h"
#include "object.h"
#include "util.h"

/**
Basic tests:
*/
void hashobject(std::vector<std::string> &args) {
    TCLAP::CmdLine cmd("hash-object", ' ', "0.1");

    // defines arguments
    TCLAP::SwitchArg writeArg("w","write","whether to write object to disk",false);
    TCLAP::ValueArg<std::string> typeArg("t","type", "type of git object", true,"blob", "type of git object");
    TCLAP::UnlabeledValueArg<std::string> pathArg("path","file path to create an object from",true,"","string");

    cmd.add(writeArg);
    cmd.add(typeArg);
    cmd.add(pathArg);
    cmd.parse(args);
    bool write = writeArg.getValue();
    std::string& type = typeArg.getValue();
    std::string& path = pathArg.getValue();

    std::cout << "type: " << type << ", path: " << path << ", write: " << write << "\n";
    // process args
    try {
        std::optional<GitRepository> repo = GitRepository::find();
        if (repo){
            // read file
            std::string fileContents = read_file(fs::path(path));
            // write to disk optionally, print hash
            std::string hash = GitObject::write(*repo, type, fileContents, write);
            std::cout << hash << "\n";
        }
    } catch (std::runtime_error& err) {
        std::cerr << err.what() << "\n";
    }
}