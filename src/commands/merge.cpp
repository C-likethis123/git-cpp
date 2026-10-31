#include "commands/merge.h"
#include "commit.h"
#include "parsers/MergeParser.h"
#include "repository.h"
#include <stdexcept>

namespace commands {
void merge(std::vector<std::string> &args) {
  const auto options = MergeParser::parse(args);
  GitRepository repo = GitRepository::find();
  // TODO: Merge options.commit into HEAD.
  const auto &commit = GitCommit::read(options.commit);
  const auto &head_commit = GitCommit::read("HEAD");

  throw std::runtime_error("Attempting to merge " + commit.get_sha() +
                           " into " + head_commit.get_sha() +
                           ", but merge is not implemented yet");
}
} // namespace commands
