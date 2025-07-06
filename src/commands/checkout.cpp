#include "commands/checkout.h"
#include <stdexcept>

#include "commit.h"
#include "object.h"
#include "parsers/CheckoutParser.h"
#include "repository.h"
#include "tree.h"
#include "util.h"

// TODO: refactoring observation: I don't need the object in multiple types
// When I query it, I already know what I want. So the factory method doesn't
// work for me here.
//
// now do performance testing. this should work better now! and it looks cleaner
// on the surface.

namespace commands {
void checkout(std::vector<std::string> &args) {
  // defines arguments
  CheckoutParser &parser = CheckoutParser::get();
  parser.parse(args);

  bool isCreateNewBranch = parser.isCreateNewBranch();
  // process args
  GitRepository repo = GitRepository::find();
  // create a new branch with the name of the commit.
  if (isCreateNewBranch) {
    std::string branchName = parser.getCommit();
    // creating at refs/heads/{branch_name}. check if the branch name exists.
    if (repo.has_branch(branchName)) {
      throw std::runtime_error("fatal: a branch named '" + branchName +
                               "' already exists.");
    }
    bool isStartPointSet = parser.isStartPointSet();
    std::string hash = GitObject::find(
        repo, isStartPointSet ? parser.getStartPoint() : "HEAD");
    create_file(repo.branch_path(branchName), hash);
    repo.update_head("ref: refs/heads/" + branchName);
  } else {
    std::string hash = parser.getCommit();
    GitCommit commit = GitCommit::read(repo, hash);
    GitTree tree = GitTree::read(repo, commit.get_tree());
    GitCommit head = GitCommit::read(repo, "HEAD");
    GitTree treeObj = GitTree::read(repo, head.get_tree());
    GitTree::instantiate_tree(tree, treeObj, repo.worktree_path(""));
    if (repo.has_object(hash)) {
      repo.update_head(hash);
    } else if (fs::exists(repo.repo_path("refs/tags/" + hash))) {
      repo.update_head("ref: refs/tags/" + hash);
    } else {
      repo.update_head("ref: refs/heads/" + hash);
    }
  }
}
} // namespace commands
