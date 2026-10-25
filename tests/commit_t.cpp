#include "catch2/catch.hpp"
#include "commit.h"
#include "repository.h"

namespace {
const std::string tree = "tree 1111111111111111111111111111111111111111\n";
const std::string identity =
    "author Test User <test@example.com> 1700000000 +0800\n"
    "committer Test User <test@example.com> 1700000001 +0800\n";
const std::string first_parent = "2222222222222222222222222222222222222222";
const std::string second_parent = "3333333333333333333333333333333333333333";
} // namespace

TEST_CASE("Commit parents and messages round trip", "[commit]") {
  GitRepository repo(".", true);
  const auto parent_count = GENERATE(0, 1, 2, 3);
  const auto message =
      GENERATE(std::string{}, std::string("subject"), std::string("subject\n"),
               std::string("subject\n\nbody\nparent not-a-header\n\n"));
  std::vector<std::string> parents;
  std::string payload = tree;
  for (int i = 0; i < parent_count; ++i) {
    parents.push_back(i == 0 ? first_parent : second_parent);
    payload += "parent " + parents.back() + "\n";
  }
  payload += identity + "\n" + message;

  GitCommit commit(payload);
  REQUIRE(commit.get_parents() == parents);
  REQUIRE(commit.has_parents() == (parent_count != 0));
  REQUIRE(commit.get_tree() == "1111111111111111111111111111111111111111");
  REQUIRE(commit.serialise(repo) == payload);
  REQUIRE(GitCommit(commit.serialise(repo)).get_parents() == parents);
}

TEST_CASE("Commit preserves signature continuation lines", "[commit]") {
  GitRepository repo(".", true);
  const auto payload = tree + "parent " + first_parent + "\n" + identity +
                       "gpgsig -----BEGIN PGP SIGNATURE-----\n"
                       " \n"
                       " signature-data\n"
                       " -----END PGP SIGNATURE-----\n\n"
                       "subject\n\nbody without final newline";
  GitCommit commit(payload);
  REQUIRE(commit.serialise(repo) == payload);
}

TEST_CASE("Deserialising a commit replaces old state", "[commit]") {
  GitRepository repo(".", true);
  GitCommit commit(tree + "parent " + first_parent + "\n" + identity +
                   "gpgsig signature\n continuation\n\nold message\n");
  const auto root = tree + identity + "\nnew message\n";
  commit.deserialise(root);
  REQUIRE(commit.get_parents().empty());
  REQUIRE_FALSE(commit.has_parents());
  REQUIRE(commit.serialise(repo) == root);
  commit.deserialise("");
  REQUIRE(commit.get_parents().empty());
}

TEST_CASE("Commit rejects malformed headers", "[commit]") {
  REQUIRE_THROWS_AS(GitCommit(tree + identity), std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit(" continuation\n\nmessage"), std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit("invalid-header\n\nmessage"), std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit(tree + "author invalid\n\nmessage"),
                    std::runtime_error);
}

TEST_CASE("Commit keeps author and committer identities separate", "[commit]") {
  GitRepository repo(".", true);
  const auto payload = tree +
      "author Original Author <author@example.com> 1600000000 -0530\n"
      "committer Other Committer <committer@example.com> 1700000001 +0800\n"
      "\nsubject\n\nbody\n";
  GitCommit commit(payload);
  REQUIRE(commit.serialise(repo) == payload);
  REQUIRE(commit.get_committer_timestamp() == 1700000001);
  REQUIRE(commit.print_commit().find("Original Author <author@example.com>") !=
          std::string::npos);
}

TEST_CASE("Commit rejects missing and duplicate required headers", "[commit]") {
  REQUIRE_THROWS_AS(GitCommit(identity + "\nmessage"), std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit(tree + "\nmessage"), std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit(tree + tree + identity + "\nmessage"),
                    std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit(tree + identity + identity + "\nmessage"),
                    std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit(tree + "parent \n" + identity + "\nmessage"),
                    std::runtime_error);
}

TEST_CASE("Commit requires a complete numeric timestamp", "[commit]") {
  const auto timestamp = GENERATE(std::string("1700000000junk"),
                                  std::string("999999999999999999999999"),
                                  std::string(""), std::string(" 1700000000"));
  const auto payload = tree + "author Test <test@example.com> " + timestamp +
                       " +0800\n"
                       "committer Test <test@example.com> 1700000001 +0800\n"
                       "\nmessage";
  REQUIRE_THROWS_AS(GitCommit(payload), std::runtime_error);
}

TEST_CASE("Commit rejects continuations on single-line headers", "[commit]") {
  REQUIRE_THROWS_AS(GitCommit(tree + " continuation\n" + identity + "\nmessage"),
                    std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit(tree + identity + " continuation\n\nmessage"),
                    std::runtime_error);
  REQUIRE_THROWS_AS(GitCommit(tree + "parent " + first_parent +
                             "\n continuation\n" + identity + "\nmessage"),
                    std::runtime_error);
}

TEST_CASE("Commit skips unfamiliar multiline headers", "[commit]") {
  const auto payload = tree + identity +
                       "extension value\n continuation\n \n\nsubject\n\nbody";
  GitCommit commit(payload);
  REQUIRE(commit.get_tree() == "1111111111111111111111111111111111111111");
  REQUIRE(commit.print_commit().find("subject\n\nbody") != std::string::npos);
}
