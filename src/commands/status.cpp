#include "commands/status.h"

#include "index.h"
#include "repository.h"
#include "tclap/CmdLine.h"
#include <string>

namespace commands {
void status(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("status", ' ', "0.1");

  // defines arguments

  cmd.ignoreUnmatched(true);
  cmd.parse(args);

  // process args
  GitRepository repo = GitRepository::find();
  const std::string branch = repo.get_head();
  std::cout << "On: " << branch << std::endl;

  GitIndex index = GitIndex::read(repo);
  index.scan_status(repo);

  // show whether it's up to date with remote branch - if it's there
  // changes to be committed
  // changes not staged for commit
  // untracked files
  // also need to support git ignores...
}
} // namespace commands
