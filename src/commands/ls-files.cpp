#include "commands/ls-files.h"

#include "index.h"
#include "parsers/LsFilesParser.h"
#include "repository.h"
#include <filesystem>
#include <iostream>
#include <string>

namespace commands {
void lsfiles(std::vector<std::string> &args) {
  LsFilesParser::parse(args);

  // process args
  GitRepository repo = GitRepository::find();
  GitIndex index_file = GitIndex::read();
  fs::path path_to_match =
      fs::relative(fs::current_path(), repo.worktree_path(""))
          .lexically_normal();

  std::string path =
      path_to_match == fs::path(".") ? "" : path_to_match.string();
  std::cout << repo.worktree_path("").string() << ", " << path << std::endl;
  index_file.print_matching_patterns(path);
}
} // namespace commands
