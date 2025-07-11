#include "commands/tag.h"
#include "object.h"
#include "parsers/TagParser.h"
#include "repository.h"
#include "util.h"

namespace commands {
void tag(std::vector<std::string> &args) {
  TagParser &parser = TagParser::get();
  parser.parse(args);
  // process args
  GitRepository repo = GitRepository::find();
  // resolve ref to find the commit hash
  std::string commit =
      parser.isCommitSet() ? parser.getCommit() : GitObject::find(repo, "HEAD");
  std::string tag = parser.getTag();
  // ERROR: if the commit doesn't exist.
  if (!repo.has_object(commit)) {
    std::string error_message = commit + ": not a valid commit";
    throw std::runtime_error(error_message);
  }
  // ERROR: if the tag already exists
  fs::path tag_path = repo.repo_path("refs/tags/" + tag);
  if (fs::is_regular_file(tag_path)) {
    std::string error_message = "tag '" + tag + "' already exists";
    throw std::runtime_error(error_message);
  }

  // create a new file in refs/tags/{name}, and write the sha of the commit.
  create_file(tag_path, commit + "\n");
}
} // namespace commands
