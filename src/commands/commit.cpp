#include "commands/commit.h"
#include "parsers/CommitParser.h"
#include "repository.h"
#include <vector>

namespace commands {
void commit(std::vector<std::string> &args) {
  const auto options = CommitParser::parse(args);
  const std::string &message = options.message;
  GitRepository repo = GitRepository::find();

  // TODO: implement
}
} // namespace commands
