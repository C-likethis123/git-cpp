#include "commands/cat-file.h"
#include <iostream>

#include "object.h"
#include "repository.h"
#include "tclap/CmdLine.h"

void catfile(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("cat-file", ' ', "0.1");

  // defines arguments
  TCLAP::UnlabeledValueArg<std::string> typeArg("type", "type of object", true,
                                                "blob", "blob");
  TCLAP::UnlabeledValueArg<std::string> objArg(
      "object", "object hash", true, "e6c0a6d3b2ca0dbb3313843238d7e27f63259d3a",
      "string");

  cmd.add(typeArg);
  cmd.add(objArg);
  cmd.parse(args);
  std::string &type = typeArg.getValue();
  std::string &hash = objArg.getValue();

  // process args
  try {
    std::optional<GitRepository> repo = GitRepository::find();
    if (repo) {
      GitObject *obj =
          GitObject::read(*repo, GitObject::find(*repo, hash, type));
      std::cout << obj->serialise(*repo) << "\n";
    }
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}