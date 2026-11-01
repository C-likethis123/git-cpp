#include "commands/status.h"

#include "index.h"
#include "parsers/StatusParser.h"
#include "repository.h"
#include <iostream>
#include <print>
#include <string>

namespace commands {
void status(std::vector<std::string> &args) {
  StatusParser::parse(args);

  // process args
  GitRepository repo = GitRepository::find();
  const std::string branch = repo.get_head();
  std::println(std::cout, "On: {}", branch);

  GitIndex index = GitIndex::read();
  index.scan_status();
}
} // namespace commands
