#include "commands/hash-object.h"
#include <iostream>

#include "object.h"
#include "repository.h"
#include "tclap/CmdLine.h"
#include "util.h"

/**
Basic tests:
*/
namespace commands {
void hashobject(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("hash-object", ' ', "0.1");

  // defines arguments
  TCLAP::SwitchArg writeArg("w", "write", "whether to write object to disk",
                            false);
  TCLAP::ValueArg<std::string> typeArg("t", "type", "type of git object", true,
                                       "blob", "type of git object");
  TCLAP::UnlabeledValueArg<std::string> pathArg(
      "path", "file path to create an object from", true, "", "string");

  cmd.add(writeArg);
  cmd.add(typeArg);
  cmd.add(pathArg);
  cmd.parse(args);
  bool write = writeArg.getValue();
  std::string &type = typeArg.getValue();
  std::string &path = pathArg.getValue();

  try {
    std::optional<GitRepository> repo = GitRepository::find();
    if (repo) {
      std::string fileContents = read_file(fs::path(path));
      // write to disk optionally, print hash
      std::string hash = GitObject::write(*repo, type, fileContents, write);
      std::cout << hash << "\n";
    }
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}
} // namespace commands
