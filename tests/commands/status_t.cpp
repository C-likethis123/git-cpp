#include "catch2/catch.hpp"
#include "commands/add.h"
#include "commands/status.h"
#include "repository.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("status command", "[status]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid git status", "nothing to commit, working tree clean") {
    std::vector<std::string> args({"status"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    REQUIRE_STDOUT_VALUE(commands::status(args),
                         "On: main\n"
                         "nothing to commit, working tree clean\n");
  }

  SECTION("Valid git status", "untracked files") {
    std::vector<std::string> args({"status"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    file_utils::create_file(VALID_GIT_PATH / "README", "test contents");
    REQUIRE_STDOUT_VALUE(commands::status(args), "On: main\n"
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
    REQUIRE_STDOUT_VALUE(commands::status(args), "On: main\n"
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
    REQUIRE_STDOUT_VALUE(commands::status(args),
                         "On: main\n"
                         "Changes not staged for commit:\n"
                         "\tmodified: test\n");
  }

  SECTION("Valid git status", "modified files, change staged") {
    std::vector<std::string> args({"status"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(GitRepoSetup::get_file_contents(".git/HEAD") ==
            "ref: refs/heads/main");
    REQUIRE(file_utils::create_file(VALID_GIT_PATH / "test",
                                    "modified tracked file contents\n"));
    std::vector<std::string> add_args({"add", "test"});
    commands::add(add_args);
    REQUIRE_STDOUT_VALUE(commands::status(args), "On: main\n"
                                                 "Changes to be committed:\n"
                                                 "\tmodified: test\n");
  }

  SECTION("Deleted files, change not staged") {
    std::vector<std::string> args({"status"});

    REQUIRE(fs::remove(VALID_GIT_PATH / "test"));
    REQUIRE_STDOUT_VALUE(commands::status(args),
                         "On: main\n"
                         "Changes not staged for commit:\n"
                         "\tdeleted: test\n");
  }

  SECTION("Deleted files, change staged") {
    std::vector<std::string> args({"status"});

    // Use Git to prepare the index: commands::add cannot stage deletions yet.
    REQUIRE(std::system(
                "git --git-dir=.git --work-tree=. rm --quiet -- test") == 0);
    REQUIRE_FALSE(fs::exists(VALID_GIT_PATH / "test"));
    REQUIRE_STDOUT_VALUE(commands::status(args), "On: main\n"
                                                 "Changes to be committed:\n"
                                                 "\tdeleted: test\n");
  }

  SECTION("Gitignore: ignored file is not reported") {
    std::vector<std::string> args({"status"});

    REQUIRE(file_utils::create_file(VALID_GIT_PATH / "test3",
                                    "ignored contents\n"));
    REQUIRE_STDOUT_VALUE(commands::status(args),
                         "On: main\n"
                         "nothing to commit, working tree clean\n");

    // A non-ignored file must still be reported alongside the ignored file.
    REQUIRE(file_utils::create_file(VALID_GIT_PATH / "README",
                                    "visible contents\n"));
    args = {"status"};
    REQUIRE_STDOUT_VALUE(commands::status(args), "On: main\n"
                                                 "Untracked files:\n"
                                                 "\tREADME\n\n");
  }

  SECTION("Paths are relative to a nested current directory") {
    const auto nested = VALID_GIT_PATH / "nested" / "child";
    fs::create_directories(nested);
    REQUIRE(file_utils::create_file(nested / "added", "staged contents\n"));
    std::vector<std::string> add_args({"add", "nested/child/added"});
    commands::add(add_args);
    REQUIRE(file_utils::create_file(VALID_GIT_PATH / "test",
                                    "modified contents\n"));
    REQUIRE(
        file_utils::create_file(nested / "untracked", "untracked contents\n"));

    fs::current_path(nested);
    std::vector<std::string> args({"status"});
    REQUIRE_STDOUT_VALUE(commands::status(args),
                         "On: main\n"
                         "Changes to be committed:\n"
                         "\tnew file: added\n\n"
                         "Changes not staged for commit:\n"
                         "\tmodified: ../../test\n\n"
                         "Untracked files:\n"
                         "\tuntracked\n\n");
  }

  SECTION("Deleted paths are relative to the current directory") {
    const auto nested = VALID_GIT_PATH / "nested";
    fs::create_directories(nested);
    const bool staged = GENERATE(false, true);
    if (staged) {
      REQUIRE(std::system(
                  "git --git-dir=.git --work-tree=. rm --quiet -- test") == 0);
    } else {
      REQUIRE(fs::remove(VALID_GIT_PATH / "test"));
    }

    fs::current_path(nested);
    std::vector<std::string> args({"status"});
    REQUIRE_STDOUT_VALUE(commands::status(args),
                         std::string("On: main\n") +
                             (staged ? "Changes to be committed:\n"
                                     : "Changes not staged for commit:\n") +
                             "\tdeleted: ../test\n");
  }
}

TEST_CASE("add and status ignore submodules", "[add][status]") {
  GitRepoSetup setup;
  const bool git_file = GENERATE(false, true);
  fs::create_directories("vendor/module/nested");
  if (git_file) {
    REQUIRE(file_utils::create_file("vendor/module/.git",
                                    "gitdir: ../../.git/modules/module\n"));
  } else {
    fs::create_directories("vendor/module/.git");
  }
  REQUIRE(file_utils::create_file("vendor/module/nested/file", "private\n"));
  REQUIRE(std::system(
              "git update-index --add --cacheinfo 160000,"
              "1723ac93b92db1fc2c28de8e5da814136937f8c6,vendor/module") == 0);

  std::vector<std::string> status_args{"status"};
  REQUIRE_STDOUT_VALUE(commands::status(status_args),
                       "On: main\nnothing to commit, working tree clean\n");

  // Include a normal file so add must succeed while skipping the submodule.
  REQUIRE(file_utils::create_file("visible", "visible\n"));
  std::vector<std::string> add_args{"add", "."};
  REQUIRE_NOTHROW(commands::add(add_args));
  const auto index = GitRepoSetup::get_file_contents(".git/index");
  REQUIRE(index.find("vendor/module") != std::string::npos);
  REQUIRE(index.find("vendor/module/nested/file") == std::string::npos);
  REQUIRE(index.find("visible") != std::string::npos);
  add_args = {"add", "vendor/module/nested/file"};
  REQUIRE_THROWS_WITH(commands::add(add_args),
                      "path vendor/module/nested/file did not match any files");

  // A gitlink in HEAD must also be ignored after its working directory is gone.
  REQUIRE(std::system("git -c user.name=Test -c user.email=test@example.com "
                      "-c core.hooksPath=/dev/null commit -qm submodule") == 0);
  fs::remove_all("vendor/module");
  status_args = {"status"};
  REQUIRE_STDOUT_VALUE(commands::status(status_args),
                       "On: main\nnothing to commit, working tree clean\n");
  REQUIRE(std::system("git update-index --force-remove vendor/module") == 0);
  status_args = {"status"};
  REQUIRE_STDOUT_VALUE(commands::status(status_args),
                       "On: main\nnothing to commit, working tree clean\n");
}
