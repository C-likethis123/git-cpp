#include "commands/show-ref.h"
#include <iostream>

#include "repository.h"
#include "tclap/CmdLine.h"
#include "util.h"

namespace commands {
void showref(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("show-ref", ' ', "0.1");

  // TODO defines arguments
  cmd.parse(args);

  // process args
  try {
    std::optional<GitRepository> repo = GitRepository::find();
    if (repo) {
      // TODO filter refs by pattern

      // recursively traverse these directories: refs/heads, refs/remotes,
      // stash, tags. basically everything in .git/refs
      // don't care about filtering first?

      // if it's a file, print the ref
      fs::path refs_path = repo->repo_path("refs");
      fs::path gitdir = repo->repo_path("");
      for (const auto &entry : fs::recursive_directory_iterator(refs_path)) {
        fs::path path = entry.path();
        if (fs::is_regular_file(path)) {
          std::string sha = resolve_ref(path, *repo);
          std::cout << sha << " " << remove_file_prefix(path, gitdir) << "\n";
        }
      }
    }
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}
} // namespace commands
