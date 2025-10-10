#include "commands/add.h"
#include "index.h"
#include "parsers/AddParser.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;
namespace commands {
void add(std::vector<std::string> &args) {
  try {
    AddParser &parser = AddParser::get();
    parser.parse(args);
    std::string path = parser.getPath();
    GitRepository repo = GitRepository::find();
    // TODO implement showing diffs
    // TODO implement partial additions

    // TODO: treat path as a pattern instead of an absolute file path
    fs::path path_relative_to_repo = fs::relative(path, repo.worktree_path(""));
    fs::path path_to_search =
        repo.worktree_path("").parent_path() / path_relative_to_repo;
    std::cout << "add: " << path_relative_to_repo << std::endl;

    // check if file exists
    if (!fs::exists(path_to_search)) {
      throw std::runtime_error("path " + path_relative_to_repo.string() +
                               " did not match any files");
    }
    // if adding a file that has no changes, no need to show an error, just
    // ignore.

    GitIndex index = GitIndex::read(repo);
    index.add_file(path_to_search, repo);
    index.save(repo);
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }
}
} // namespace commands
