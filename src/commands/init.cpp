#include "commands/init.h"
#include <iostream>
#include <print>

#include "repository.h"
#include "parsers/InitParser.h"

namespace commands {
void init(std::vector<std::string> &args) {
  const auto options = InitParser::parse(args);

  // process args
  GitRepository repo(options.path, true);
  std::string worktree = repo.create(false);
  std::println(std::cout, "Initialised Git repo at {}", worktree);
}
} // namespace commands
