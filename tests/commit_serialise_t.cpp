#include "catch2/catch.hpp"
#include "commit.h"
#include "repository.h"

TEST_CASE("New commits use the shared serializer", "[commit]") {
  GitRepository repo(".", true);
  const std::string tree = "tree 1111111111111111111111111111111111111111\n";
  const std::string first_parent(40, '2');
  const std::string second_parent(40, '3');
  const std::string identity =
      "author Test User <test@example.com> 1700000000 +0800\n"
      "committer Test User <test@example.com> 1700000001 +0800\n";
  const GitCommit::Identity author{"Test User", "test@example.com", 1700000000,
                                   "+0800"};
  const GitCommit::Identity committer{"Test User", "test@example.com",
                                      1700000001, "+0800"};
  GitCommit commit("1111111111111111111111111111111111111111",
                   {first_parent, second_parent}, author, committer,
                   "subject\n\nbody\n");
  CHECK(commit.serialise() == tree + "parent " + first_parent + "\nparent " +
                                  second_parent + "\n" + identity +
                                  "\nsubject\n\nbody\n");
}
