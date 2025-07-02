#include "commands/cat-file.h"
#include <iostream>

#include "object.h"
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
  std::cout << obj->serialise(repo);
}
} // namespace commands
