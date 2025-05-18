#include "catch2/catch.hpp"
#include "commands/tag.h"
#include "repository.h"
#include "utils/gitreposetup.h"
#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("tag command", "[tag]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid git tag command - tag name only", "tag name only") {
    std::vector<std::string> args({"tag", "v1.0"});
    commands::tag(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(fs::exists(".git/refs/tags/v1.0"));
    REQUIRE(GitRepoSetup::get_file_contents(".git/refs/tags/v1.0") ==
            SECOND_COMMIT);
  }
  SECTION("Valid git tag command - tag name with commit",
          "tag name with commit") {
    std::vector<std::string> args({"tag", "v1.0", SECOND_COMMIT});
    commands::tag(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(fs::exists(".git/refs/tags/v1.0"));
    REQUIRE(GitRepoSetup::get_file_contents(".git/refs/tags/v1.0") ==
            SECOND_COMMIT);
  }
}

TEST_CASE("tag with errors", "[tag errors]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Git tag already exists") {
    std::vector<std::string> args({"tag", "v1.0"});
    commands::tag(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(fs::exists(".git/refs/tags/v1.0"));
    REQUIRE(GitRepoSetup::get_file_contents(".git/refs/tags/v1.0") ==
            SECOND_COMMIT);

    std::vector<std::string> args2({"tag", "v1.0"});
    REQUIRE_THROWS_WITH(commands::tag(args2), "tag 'v1.0' already exists");
  }
  SECTION("Git tag failed with non-existent commit") {
    std::vector<std::string> args({"tag", "v1.0", "nonexistentcommit"});
    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));

    REQUIRE_THROWS_WITH(commands::tag(args),
                        "nonexistentcommit: not a valid commit");
    REQUIRE(!fs::exists(".git/refs/tags/v1.0"));
  }
}
