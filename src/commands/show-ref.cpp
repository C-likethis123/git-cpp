#include "commands/show-ref.h"
#include <iostream>

#include "parsers/ShowRefParser.h"
#include "repo_util.h"
#include "repository.h"
#include "util.h"

namespace commands {
void showref(std::vector<std::string> &args) {
  ShowRefParser::parse(args);

  // process args
  GitRepository repo = GitRepository::find();
  // TODO filter refs by pattern

  // recursively traverse these directories: refs/heads, refs/remotes,
  // stash, tags. basically everything in .git/refs
  // don't care about filtering first?

  // if it's a file, print the ref
  fs::path refs_path = repo.repo_path("refs");
  fs::path gitdir = repo.repo_path("");
  for (const auto &entry : fs::recursive_directory_iterator(refs_path)) {
    fs::path path = entry.path();
    if (fs::is_regular_file(path)) {
      std::string sha = resolve_ref(path);
      std::cout << sha << " " << remove_file_prefix(path, gitdir) << "\n";
    }
  }
}
} // namespace commands
