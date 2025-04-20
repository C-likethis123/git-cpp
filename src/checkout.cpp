#include "commands/checkout.h"
#include <iostream>

#include "commit.h"
#include "object.h"
#include "repository.h"
#include "tclap/CmdLine.h"
#include "tree.h"

// todo FIX ERROR OPENING THE FILE FOR READING... bug
namespace commands {
void checkout(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("checkout", ' ', "0.1");

  // defines arguments
  TCLAP::UnlabeledValueArg<std::string> commitArg(
      "commit", "hash of the commit to checkout to", true, "HEAD",
      "commit hash");

  cmd.add(commitArg);
  cmd.parse(args);
  std::string &hash = commitArg.getValue();

  // process args
  try {
    std::optional<GitRepository> repo = GitRepository::find();
    if (repo) {
      GitCommit *commit =
          dynamic_cast<GitCommit *>(GitObject::read(*repo, hash));
      if (!commit) {
        throw std::runtime_error("Invalid commit object: " + hash);
      }
      GitTree *tree =
          dynamic_cast<GitTree *>(GitObject::read(*repo, commit->get_tree()));
      if (!tree) {
        throw std::runtime_error("Invalid tree object: " + commit->get_tree());
      }
      std::string orig_head = GitObject::find(*repo, "HEAD");
      GitCommit *head =
          dynamic_cast<GitCommit *>(GitObject::read(*repo, orig_head));
      GitTree *treeObj =
          dynamic_cast<GitTree *>(GitObject::read(*repo, head->get_tree()));
      // TODO: do an entry by entry comparison of both trees, then dump the
      // files out to the working directory

      GitTree::instantiate_tree(tree, treeObj, repo->worktree_path(""));
      // TODO: fix bug in this one
      repo->update_head(hash);
    }
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}
} // namespace commands
