#include "catch2/catch.hpp"
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
  //   SECTION("Valid git status", "nothing to commit, working tree clean") {
  //     std::vector<std::string> args({"status"});
  //     commands::status(args);

  //     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //     REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") == "HEAD");
  //     REQUIRE_STDOUT_VALUE(
  //         commands::status(args),
  //         "On: main\n\nNo commits yet\n\nUntracked "
  //         "files:\n\tREADME\n\nnothing added to commit but untracked files
  //         " "present (use \"git add\" to track)\n");
  //   }

  SECTION("Valid git status", "untracked files") {
    std::vector<std::string> args({"status"});
    commands::status(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    file_utils::create_file(VALID_GIT_PATH / "README", "test contents");
    REQUIRE_STDOUT_VALUE(commands::status(args),
                         "On: main\n\nChanges not staged for commit:\n\n"
                         "untracked: README\n");
  }

  //   SECTION("Valid git status", "new file, change staged") {
  //     std::vector<std::string> args({"status"});
  //     commands::status(args);

  //     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //     REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") == "HEAD");
  //     REQUIRE_STDOUT_VALUE(
  //         commands::status(args),
  //         "On: main\n\nUntracked "
  //         "files:\n\tREADME\n\nnothing added to commit but untracked files
  //         " "present (use \"git add\" to track)\n");
  //   }

  //   SECTION("Valid git status", "modified files, change not staged") {
  //     std::vector<std::string> args({"status"});
  //     commands::status(args);

  //     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //     REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") == "HEAD");
  //     REQUIRE_STDOUT_VALUE(
  //         commands::status(args),
  //         "On: main\n\nUntracked "
  //         "files:\n\tREADME\n\nnothing added to commit but untracked files
  //         " "present (use \"git add\" to track)\n");
  //   }

  //   SECTION("Valid git status", "modified files, change staged") {
  //     std::vector<std::string> args({"status"});
  //     commands::status(args);

  //     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //     REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") == "HEAD");
  //     REQUIRE_STDOUT_VALUE(
  //         commands::status(args),
  //         "On: main\n\nUntracked "
  //         "files:\n\tREADME\n\nnothing added to commit but untracked files
  //         " "present (use \"git add\" to track)\n");
  //   }

  //   SECTION("Valid git status", "deleted files, change not staged") {
  //     std::vector<std::string> args({"status"});
  //     commands::status(args);

  //     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //     REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") == "HEAD");
  //   }

  //   SECTION("Valid git status", "deleted files, change staged") {
  //     std::vector<std::string> args({"status"});
  //     commands::status(args);

  //     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //     REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") == "HEAD");
  //   }

  //   SECTION("Gitignore", "file in gitignore not detected") {
  //     std::vector<std::string> args({"status"});
  //     commands::status(args);

  //     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  //     REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") == "HEAD");
  //   }
}
