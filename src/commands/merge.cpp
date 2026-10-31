#include "commands/merge.h"
#include "commit.h"
#include "parsers/MergeParser.h"
#include "repository.h"
#include <iostream>
#include <stdexcept>

namespace commands {
void merge(std::vector<std::string> &args) {
  const auto options = MergeParser::parse(args);
  GitRepository repo = GitRepository::find();
  // TODO: Merge options.commit into HEAD.
  const auto &commit = GitCommit::read(options.commit);
  const auto &head_commit = GitCommit::read("HEAD");

  // Fast forward case, just update head
  if (commit.has_ancestor(head_commit)) {
    repo.update_current_ref(commit.get_sha());
    std::cout << "Fast-forward HEAD to " << commit.get_sha() << std::endl;
  } else {
    // merge two trees
    // case 1: declare merge conflict and stop merging
    // case 2: create merge commit, update refs
    throw std::runtime_error("Attempting to merge " + commit.get_sha() +
                             " into " + head_commit.get_sha() +
                             ", but merge is not implemented yet");
  }
}
} // namespace commands
