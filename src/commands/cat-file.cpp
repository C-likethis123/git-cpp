#include "commands/cat-file.h"
#include <iostream>

#include "commit.h"
#include "object.h"
#include "parsers/CatfileParser.h"

namespace commands {
void catfile(std::vector<std::string> &args) {
  const auto options = CatfileParser::parse(args);
  const auto type = options.type;
  std::string hash = options.hash;

  GitObject *obj = GitObject::read(GitObject::find(hash));
  const auto obj_type = obj->get_type();
  if (type == GitObjectType::Tree && obj_type == GitObjectType::Commit) {
    GitCommit commitObj = GitCommit::read(hash);
    std::string treeHash = commitObj.get_tree();
    GitObject *treeObj = GitObject::read(treeHash);
    std::cout << treeObj->serialise();
  } else {
    std::cout << obj->serialise();
  }
}
} // namespace commands
