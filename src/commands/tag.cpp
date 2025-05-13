#include "commands/tag.h"
#include "object.h"
#include "repository.h"
#include "tclap/CmdLine.h"
#include "util.h"
#include <iostream>
#include <optional>

namespace commands {
void tag(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("tag", ' ', "0.1");

  // defines arguments
  TCLAP::UnlabeledValueArg<std::string> tagArg("tagname", "Tag name", true,
                                               "HEAD", "tag name");
  TCLAP::UnlabeledValueArg<std::string> commitArg(
      "commit", "Commit to tag to. If not specified, defaults to HEAD", false,
      "HEAD", "commit to tag");
  cmd.ignoreUnmatched(true);
  cmd.add(tagArg);
  cmd.add(commitArg);
  cmd.parse(args);
  // process args
  try {
    // TODO this
    std::optional<GitRepository> repo = GitRepository::find();
    if (!repo) {
      throw std::runtime_error("Not a git repository");
    }
    // resolve ref, basically find the commit object

    std::string commit = commitArg.isSet() ? commitArg.getValue()
                                           : GitObject::find(*repo, "HEAD");
    std::string tag = tagArg.getValue();
    // ERROR: if the commit doesn't exist.
    if (fs::is_regular_file(get_commit_path(commit))) {
      throw std::runtime_error("Not a valid commit");
    }
    // ERROR: if the tag already exists
    fs::path tag_path = repo->repo_path("refs/tags/" + tag);
    if (fs::is_regular_file(tag_path)) {
      throw std::runtime_error("Tag already exists");
    }

    // create a new file in refs/tags/{name}, and write the sha of the commit.
    create_file(tag_path, commit + "\n");
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}
} // namespace commands
