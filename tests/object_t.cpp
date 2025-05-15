#include "catch2/catch.hpp"
#include "object.h"
#include "repository.h"

TEST_CASE("GitObject", "[GitObject::find]") {
  GitRepoSetup gitRepoSetup;
  SECTION("find: resolves a commit") {
    // Provide a path to a valid git repository
    GitRepository repo(".");

    REQUIRE(GitObject::find(repo, "HEAD") == FIRST_COMMIT);
    REQUIRE(GitObject::find(repo, SECOND_COMMIT) == SECOND_COMMIT);
    REQUIRE(GitObject::find(repo, "tagv1.0") == SECOND_COMMIT);
    REQUIRE(GitObject::find(repo, "refs/heads/test_branch") == THIRD_COMMIT);
  }
}

TEST_CASE("GitRepository Create", "[GitRepository]") {}
