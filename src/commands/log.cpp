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
  try {
    std::optional<GitRepository> repo = GitRepository::find();
    if (repo) {
      std::string commit = commitArg.getValue();
      GitCommit *commitObj;

      do {
        GitObject *obj = GitObject::read(*repo, commit);
        commitObj = dynamic_cast<GitCommit *>(obj);
        if (commitObj) {
          std::cout << commitObj->print_commit() << "\n";
          commit = commitObj->get_parent();
        }
      } while (commitObj && commitObj->has_parent());
    }
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}
} // namespace commands
