#include "commands/cat-file.h"
#include <iostream>

#include "object.h"
#include "commit.h"
#include "parsers/CatfileParser.h"
#include "repository.h"

namespace commands {
void catfile(std::vector<std::string> &args) {
  const auto options = CatfileParser::parse(args);
  const auto type = options.type;
  std::string hash = options.hash;

  GitRepository repo = GitRepository::find();
  GitObject *obj = GitObject::read(repo, GitObject::find(repo, hash));
  const auto obj_type = obj->get_type();
  if (type == GitObjectType::Tree && obj_type == GitObjectType::Commit) {
    GitCommit commitObj = GitCommit::read(repo, hash);
    std::string treeHash = commitObj.get_tree();
    GitObject *treeObj = GitObject::read(repo, GitObject::find(repo, treeHash));
    std::cout << treeObj->serialise(repo);
  } else {
      std::cout << obj->serialise(repo);
  }
}
} // namespace commands
