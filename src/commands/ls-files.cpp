#include "commands/ls-files.h"

#include "index.h"
#include "repository.h"
#include "tclap/CmdLine.h"
#include <filesystem>
#include <string>

namespace commands {
void lsfiles(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("ls-files", ' ', "0.1");

  // defines arguments

  cmd.ignoreUnmatched(true);
  cmd.parse(args);

  // process args
  GitRepository repo = GitRepository::find();
  GitIndex index_file = GitIndex::read(repo);
  fs::path path_to_match =
      fs::relative(fs::current_path(), repo.worktree_path(""))
          .lexically_normal();

  std::string path =
      path_to_match == fs::path(".") ? "" : path_to_match.string();
  std::cout << repo.worktree_path("").string() << ", " << path << std::endl;
  index_file.print_matching_patterns(repo, path);
}
} // namespace commands
