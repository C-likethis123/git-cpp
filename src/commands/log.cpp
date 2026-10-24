#include "commands/log.h"
#include "commit.h"
#include "object.h"
#include <iostream>

#include "parsers/LogParser.h"
#include "repository.h"

namespace commands {
void log(std::vector<std::string> &args) {
  const auto options = LogParser::parse(args);

  // process args
  GitRepository repo = GitRepository::find();
  std::string commit = GitObject::find(repo, options.commit);
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
