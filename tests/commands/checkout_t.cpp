#include "catch2/catch.hpp"
#include "commands/checkout.h"
#include "repository.h"
#include "utils/gitreposetup.h"
#include <filesystem>

// Helper function to read the contents of a file

TEST_CASE("checkout command", "[checkout]") {
  GitRepoSetup gitRepoSetup;
  SECTION("checkout existing branch", "tag name only") {
    std::vector<std::string> args({"checkout", "test_branch"});
    commands::checkout(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/test_branch");
    REQUIRE(GitRepository(VALID_GIT_PATH).get_head() == "test_branch");
  }
  SECTION("checkout existing commit", "tag name with commit") {
    std::vector<std::string> args({"checkout", SECOND_COMMIT});
    commands::checkout(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    // check file contents match second commit
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") == SECOND_COMMIT);

    GitRepository repo(VALID_GIT_PATH);

    // Use the create function
    REQUIRE(repo.get_head() == SECOND_COMMIT);
  }
  SECTION("checkout existing tag", "tag name with commit") {
    std::vector<std::string> args({"checkout", TEST_TAG});
    commands::checkout(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    // check file contents match second commit
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/tags/test_tag");
    REQUIRE(GitRepository(VALID_GIT_PATH).get_head() == TEST_TAG);
  }
  SECTION("checkout new branch that didn't exist before") {
    std::vector<std::string> args({"checkout", "-b", "new_branch"});
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    commands::checkout(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    // check that the new branch was created
    // TODO check that file contents of the worktree matches the commit

    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/new_branch");
    REQUIRE(GitRepoSetup::get_file_contents(".git/refs/heads/new_branch") ==
            HEAD_COMMIT);
  }
  SECTION("checkout new branch that didn't exist before with a specified start "
          "point") {
    std::vector<std::string> args(
        {"checkout", "-b", "new_branch", FIRST_COMMIT});
    commands::checkout(args);
    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    // check that the new branch was created
    // TODO check that file contents of the worktree matches the commit
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/new_branch");
    REQUIRE(GitRepoSetup::get_file_contents(".git/refs/heads/new_branch") ==
            FIRST_COMMIT);
    REQUIRE(GitRepository(VALID_GIT_PATH).get_head() == "new_branch");
  }
}

TEST_CASE("checkout with errors", "[checkout errors]") {
  GitRepoSetup gitRepoSetup;
  SECTION("branch already exists") {
    std::vector<std::string> args({"checkout", "-b", "test_branch"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE_THROWS_WITH(commands::checkout(args),
                        "fatal: a branch named 'test_branch' already exists.");
    REQUIRE(GitRepository(VALID_GIT_PATH).get_head() == "main");
  }
}
