#include "commands/init.h"
#include <iostream>

#include "repository.h"
#include "tclap/CmdLine.h"

namespace commands {
void init(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("init", ' ', "0.1");

  // defines arguments
  TCLAP::ValueArg<std::string> pathArg(
      "p", "path", "Path to create the git repo", false, ".", "string");
  cmd.ignoreUnmatched(true);
  cmd.add(pathArg);
  cmd.parse(args);
  std::string path = pathArg.getValue();

  // process args
  GitRepository repo(path, true);
  std::string worktree = repo.create(false);
  std::cout << "Initialised Git repo at " << worktree << "\n";
}
} // namespace commands
