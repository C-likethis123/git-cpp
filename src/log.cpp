#include "commands/log.h"
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
      // TODO: this part
      // from the argument, find the object.
      // if it is a commit, print the commit.
      // if the commit has parents, print out the parents.

      // TODO: figure out how to display if a commit has multiple parents
    }
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}

/*
How to print out a commit:
commit [commit hash] (HEAD ->....)
Author
Date
Message
*/