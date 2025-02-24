#include "commands/log.h"
#include "commit.h"
#include "object.h"
#include <iostream>

#include "repository.h"
#include "tclap/CmdLine.h"

void log(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("log", ' ', "0.1");

  // defines arguments
  TCLAP::UnlabeledValueArg<std::string> commitArg(
      "commit", "Display history of given commit", false, "HEAD",
      "commit to start at");
  cmd.add(commitArg);
  // process args
  try {
    std::optional<GitRepository> repo = GitRepository::find();
    if (repo) {
      std::string commit = commitArg.getValue();
      // from the argument, find the object.
      GitObject *obj = GitObject::read(*repo, commit);
      GitCommit *commitObj = dynamic_cast<GitCommit *>(obj);
      commitObj->print_commit(*repo);
      // if the commit has parents, print out the parents.
      while (commitObj->has_parent()) {
        std::string parent = commitObj->get_parent();
        GitObject *parentObj = GitObject::read(*repo, parent);
        commitObj = dynamic_cast<GitCommit *>(parentObj);
        commitObj->print_commit(*repo);
      }
    }
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}
