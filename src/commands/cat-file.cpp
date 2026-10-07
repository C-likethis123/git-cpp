#include "commands/cat-file.h"
#include <iostream>

#include "object.h"
#include "commit.h"
#include "parsers/CatfileParser.h"
#include "repository.h"

namespace commands {
void catfile(std::vector<std::string> &args) {
  CatfileParser &parser = CatfileParser::get();
  parser.parse(args);
  std::string type = parser.getType();
  std::string hash = parser.getHash();

  GitRepository repo = GitRepository::find();
  GitObject *obj = GitObject::read(repo, GitObject::find(repo, hash));
  std::string obj_type = obj->get_type();
  if (type == "tree" && obj_type == "commit") {
    GitCommit commitObj = GitCommit::read(repo, hash);
    std::string treeHash = commitObj.get_tree();
    GitObject *treeObj = GitObject::read(repo, GitObject::find(repo, treeHash));
    std::cout << treeObj->serialise(repo);
  } else {
      std::cout << obj->serialise(repo);
  }
}
} // namespace commands
