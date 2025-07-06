#include "commands/log.h"
#include "commit.h"
#include "object.h"
#include <iostream>

#include "repository.h"
#include "tclap/CmdLine.h"

namespace commands {
void log(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("log", ' ', "0.1");

  // defines arguments
  TCLAP::UnlabeledValueArg<std::string> commitArg(
      "commit", "Display history of given commit", false, "HEAD",
      "commit to start at");
  cmd.ignoreUnmatched(true);
  cmd.add(commitArg);
  cmd.parse(args);
  // process args
  GitRepository repo = GitRepository::find();
  std::string commit = GitObject::find(repo, commitArg.getValue());
  GitCommit commitObj = GitCommit::read(repo, commit);
  bool hasCommit = true;
  while (hasCommit) {
    std::cout << commitObj.print_commit() << "\n";
    if (commitObj.has_parent()) {
      commitObj = GitCommit::read(repo, commitObj.get_parent());
    } else {
      hasCommit = false;
    }
  }
}
} // namespace commands
