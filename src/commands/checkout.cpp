#include "commands/checkout.h"
#include <stdexcept>

#include "commit.h"
#include "object.h"
#include "parsers/CheckoutParser.h"
#include "repository.h"
#include "tree.h"
#include "util.h"

namespace commands {
void checkout(std::vector<std::string> &args) {
  // defines arguments
  CheckoutParser &parser = CheckoutParser::get();
  parser.parse(args);

  bool isCreateNewBranch = parser.isCreateNewBranch();
  // process args
  std::optional<GitRepository> repo = GitRepository::find();
  if (!repo) {
    throw std::runtime_error("No git repository found");
  }
  // create a new branch with the name of the commit.
  if (isCreateNewBranch) {
    std::string branchName = parser.getCommit();
    // creating at refs/heads/{branch_name}. check if the branch name exists.
    if (repo->has_branch(branchName)) {
      throw std::runtime_error("fatal: a branch named '" + branchName +
                               "' already exists.");
    }
    bool isStartPointSet = parser.isStartPointSet();
    std::string hash = GitObject::find(
        *repo, isStartPointSet ? parser.getStartPoint() : "HEAD");
    create_file(repo->branch_path(branchName), hash);
    repo->update_head("ref: refs/heads/" + branchName);
  } else {
    std::string hash = parser.getCommit();
    GitCommit *commit = dynamic_cast<GitCommit *>(
        GitObject::read(*repo, GitObject::find(*repo, hash)));
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
    GitTree::instantiate_tree(tree, treeObj, repo->worktree_path(""));
    // TODO: fix bug in this one
    if (fs::exists(repo->repo_path(get_commit_path(hash)))) {
      repo->update_head(hash);
    } else if (fs::exists(repo->repo_path("refs/tags/" + hash))) {
      repo->update_head("ref: refs/tags/" + hash);
    } else {
      repo->update_head("ref: refs/heads/" + hash);
    }
  }
}
} // namespace commands
