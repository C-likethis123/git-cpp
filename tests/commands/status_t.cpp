#include "catch2/catch.hpp"
#include "commands/add.h"
#include "commands/status.h"
#include "repository.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TEST_CASE("status command", "[status]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid git status", "nothing to commit, working tree clean") {
    std::vector<std::string> args({"status"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    REQUIRE_STDOUT_VALUE(
        commands::status(args),
        "On: main\n"
        "nothing to commit, working tree clean\n");
  }

  SECTION("Valid git status", "untracked files") {
    std::vector<std::string> args({"status"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    file_utils::create_file(VALID_GIT_PATH / "README", "test contents");
    REQUIRE_STDOUT_VALUE(
        commands::status(args),
        "On: main\n"
        "Untracked files:\n"
        "\tREADME\n\n");
  }

  SECTION("Valid git status", "new file, change staged") {
    std::vector<std::string> args({"status"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    file_utils::create_file(VALID_GIT_PATH / "README", "test contents");
    // add file to index
    std::vector<std::string> add_args({"status", "README"});
    commands::add(add_args);
    REQUIRE_STDOUT_VALUE(
        commands::status(args),
        "On: main\n"
        "Changes to be committed:\n"
        "\tnew file: README\n");
  }

  SECTION("Valid git status", "modified files, change not staged") {
    std::vector<std::string> args({"status"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    REQUIRE(file_utils::create_file(VALID_GIT_PATH / "test",
                                    "modified tracked file contents\n"));
    REQUIRE_STDOUT_VALUE(
        commands::status(args),
        "On: main\n"
        "Changes not staged for commit:\n"
        "\tmodified: test\n");
  }

  SECTION("Valid git status", "modified files, change staged") {
    std::vector<std::string> args({"status"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    REQUIRE_STDOUT_VALUE(
        commands::status(args),
        "On: main\n"
        "Untracked files:\n"\tREADME\n\nnothing added to commit but untracked files");
  }

  // SECTION("Valid git status", "deleted files, change not staged") {
  //   std::vector<std::string> args({"status"});

  //   REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //   REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
  //           "ref: refs/heads/main");
  // }

  // SECTION("Valid git status", "deleted files, change staged") {
  //   std::vector<std::string> args({"status"});

  //   REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //   REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
  //           "ref: refs/heads/main");
  // }

  // SECTION("Gitignore", "file in gitignore not detected") {
  //   std::vector<std::string> args({"status"});

  //   REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //   REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
  //           "ref: refs/heads/main");
  // }
}
