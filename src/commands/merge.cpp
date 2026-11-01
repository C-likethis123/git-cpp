#include "commands/merge.h"
#include "commit.h"
#include "index.h"
#include "parsers/MergeParser.h"
#include "repository.h"
#include "tree.h"
#include <format>
#include <iostream>
#include <print>
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
    // bug: index is not updated yet
    const auto &tree = GitTree::read(commit.get_tree());
    const auto &curr_tree = GitTree::read(head_commit.get_tree());
    GitTree::instantiate_tree(tree, curr_tree, repo.worktree_path("."));
    GitIndex index = GitIndex::from_tree(tree);
    index.save();
    repo.update_current_ref(commit.get_sha());
    std::println(std::cout, "Fast-forward HEAD to {}", commit.get_sha());
  } else {
    // merge two trees
    // case 1: declare merge conflict and stop merging
    // case 2: create merge commit, update refs
    throw std::runtime_error(std::format(
        "Attempting to merge {} into {}, but merge is not implemented yet",
        commit.get_sha(), head_commit.get_sha()));
  }
}
} // namespace commands
