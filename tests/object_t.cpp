#include "catch2/catch.hpp"
#include "object.h"
#include "repository.h"
#include "utils/gitreposetup.h"

TEST_CASE("GitObject", "[GitObject::find]") {
  GitRepoSetup gitRepoSetup;
  SECTION("find: resolves a commit") {
    // Provide a path to a valid git repository
    GitRepository repo(".");

    REQUIRE(GitObject::find("HEAD") == HEAD_COMMIT);
    REQUIRE(GitObject::find(SECOND_COMMIT) == SECOND_COMMIT);
    REQUIRE(GitObject::find("tagv1.0") == SECOND_COMMIT);
    REQUIRE(GitObject::find("test_branch") == THIRD_COMMIT);
  }
}

TEST_CASE("Git object types round trip through their header names",
          "[GitObjectType]") {
  for (const auto type :
       {GitObjectType::Blob, GitObjectType::Tree, GitObjectType::Commit}) {
    REQUIRE(git_object_type_from_string(to_string(type)) == type);
  }
  REQUIRE(to_string(GitObjectType::Blob) == "blob");
  REQUIRE(to_string(GitObjectType::Tree) == "tree");
  REQUIRE(to_string(GitObjectType::Commit) == "commit");
  REQUIRE_THROWS(git_object_type_from_string("invalid"));
  REQUIRE_THROWS(to_string(static_cast<GitObjectType>(-1)));
}
