#include "catch2/catch.hpp"
#include "repo_util.h"
#include "repository.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"

TEST_CASE("resolve loose and packed references", "[resolve_ref]") {
  GitRepoSetup setup;
  GitRepository repo(VALID_GIT_PATH.string());
  const auto branch = repo.repo_path("refs/heads/main");
  file_utils::create_file(repo.repo_path("HEAD"), "ref: refs/heads/main\n");
  file_utils::create_file(repo.repo_path("packed-refs"),
                          "# pack-refs with: peeled fully-peeled sorted\n\n" +
                              FIRST_COMMIT + " refs/heads/main-extra\n^" +
                              SECOND_COMMIT + "\n" + HEAD_COMMIT +
                              " refs/heads/main\n");

  SECTION("HEAD follows a packed branch with an exact name match") {
    fs::remove(branch);
    REQUIRE(resolve_ref(repo.repo_path("HEAD")) == HEAD_COMMIT);
  }
  SECTION("loose refs take precedence") {
    file_utils::create_file(branch, FIRST_COMMIT + "\n");
    REQUIRE(resolve_ref(repo.repo_path("HEAD")) == FIRST_COMMIT);
  }
  SECTION("missing refs report an error") {
    REQUIRE_THROWS_AS(resolve_ref(repo.repo_path("refs/heads/missing")),
                      std::runtime_error);
  }
  SECTION("malformed records report an error") {
    fs::remove(branch);
    file_utils::create_file(repo.repo_path("packed-refs"), "invalid\n");
    REQUIRE_THROWS_AS(resolve_ref(branch), std::runtime_error);
  }
  SECTION("missing packed-refs reports an unresolved reference") {
    fs::remove(branch);
    fs::remove(repo.repo_path("packed-refs"));
    REQUIRE_THROWS_AS(resolve_ref(branch), std::runtime_error);
  }
}
