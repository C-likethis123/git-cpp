#include "catch2/catch.hpp"
#include "commands/merge.h"
#include "commit.h"
#include "index.h"
#include "parsers/MergeParser.h"
#include "util.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"

TEST_CASE("merge parser accepts one target", "[merge]") {
  REQUIRE(MergeParser::parse({"merge", "feature"}).commit == "feature");
  REQUIRE(MergeParser::parse({"merge", "abc123"}).commit == "abc123");
}

TEST_CASE("merge parser rejects missing or extra arguments", "[merge]") {
  REQUIRE_THROWS(MergeParser::parse({"merge"}));
  REQUIRE_THROWS(MergeParser::parse({"merge", "feature", "other"}));
  REQUIRE_THROWS(MergeParser::parse({"merge", "--unknown", "feature"}));
}

TEST_CASE("merge command reports the unimplemented scaffold", "[merge]") {
  GitRepoSetup setup;
  const auto head = GitCommit::read("HEAD");
  const GitCommit::Identity identity{"Merge Test", "merge@example.com",
                                     1700000000, "+0800"};
  GitCommit target(head.get_tree(), {}, identity, identity, "unrelated target");
  const auto target_hash =
      GitObject::write(GitObjectType::Commit, target.serialise(), true);
  std::vector<std::string> args = {"merge", target_hash};
  REQUIRE_THROWS_WITH(commands::merge(args),
                      "Attempting to merge " + target_hash + " into " +
                          head.get_sha() + ", but merge is not implemented yet");
  REQUIRE(GitObject::find("HEAD") == head.get_sha());
}

TEST_CASE("merge command works for fast-forward merges", "[merge]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  const auto original_head = read_file(repo.repo_path("HEAD"));
  const auto target = GitObject::find("origin/merge-fast-forward");
  std::vector<std::string> args = {"merge", "origin/merge-fast-forward"};

  REQUIRE_STDOUT_VALUE(commands::merge(args),
                       "Fast-forward HEAD to " + target + "\n");
  REQUIRE(GitObject::find("HEAD") == target);
  REQUIRE(read_file(repo.repo_path("HEAD")) == original_head);
}

TEST_CASE("merge command throws for conflicting edits", "[merge]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  const auto ours = GitObject::find("origin/merge-ours");
  const auto theirs = GitObject::find("origin/merge-theirs");
  repo.update_current_ref(ours);
  create_file(repo.worktree_path("test"), "ours\n");
  auto index = GitIndex::read();
  index.add_file("test");
  index.save();
  const auto original_index = read_file(repo.repo_path("index"));
  std::vector<std::string> args = {"merge", "origin/merge-theirs"};

  // shortcut: divergent histories share this error until conflict detection exists.
  REQUIRE_THROWS_WITH(commands::merge(args),
                      "Attempting to merge " + theirs + " into " + ours +
                          ", but merge is not implemented yet");
  REQUIRE(GitObject::find("HEAD") == ours);
  REQUIRE(read_file(repo.repo_path("index")) == original_index);
  REQUIRE(read_file(repo.worktree_path("test")) == "ours\n");
}
