#include "commands/check-ignore.h"
#include <iostream>

#include "commit.h"
#include "object.h"
#include "parsers/CheckIgnoreParser.h"
#include "repository.h"

namespace commands {
void checkignore(std::vector<std::string> &args) {
  const auto options = CheckIgnoreParser::parse(args);
  const std::vector<std::string> paths = options.filePathPatterns;

  GitRepository repo = GitRepository::find();
  for (auto &path : paths) {
    if (repo.is_ignored(path)) {
      std::cout << path << std::endl;
    }
  }
}
} // namespace commands
