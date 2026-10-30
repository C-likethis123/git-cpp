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
  const auto options = CheckoutParser::parse(args);

  bool isCreateNewBranch = options.createNewBranch;
  // process args
  GitRepository repo = GitRepository::find();
  // create a new branch with the name of the commit.
  if (isCreateNewBranch) {
    std::string branchName = options.commit;
    // creating at refs/heads/{branch_name}. check if the branch name exists.
    if (repo.has_branch(branchName)) {
      throw std::runtime_error("fatal: a branch named '" + branchName +
                               "' already exists.");
    }
    bool isStartPointSet = options.startPointSet;
    std::string hash =
        GitObject::find(isStartPointSet ? options.startPoint : "HEAD");
    create_file(repo.branch_path(branchName), hash);
    repo.update_head("ref: refs/heads/" + branchName);
  } else {
    std::string hash = options.commit;
    GitCommit commit = GitCommit::read(hash);
    GitTree tree = GitTree::read(commit.get_tree());
    GitCommit head = GitCommit::read("HEAD");
    GitTree treeObj = GitTree::read(head.get_tree());
    GitTree::instantiate_tree(tree, treeObj, repo.worktree_path(""));
    if (repo.has_loose_object(hash)) {
      repo.update_head(hash);
    } else if (repo.has_tag(hash)) {
      repo.update_head("ref: refs/tags/" + hash);
    } else {
      repo.update_head("ref: refs/heads/" + hash);
    }
  }
}
} // namespace commands
