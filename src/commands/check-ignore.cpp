#include "commands/check-ignore.h"
#include <iostream>
#include <print>

#include "parsers/CheckIgnoreParser.h"
#include "repository.h"

namespace commands {
void checkignore(std::vector<std::string> &args) {
  const auto options = CheckIgnoreParser::parse(args);
  const std::vector<std::string> paths = options.filePathPatterns;

  GitRepository repo = GitRepository::find();
  for (auto &path : paths) {
    if (repo.is_ignored(path)) {
      std::println(std::cout, "{}", path);
    }
  }
}
} // namespace commands
