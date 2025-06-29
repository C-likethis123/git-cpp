#include "catch2/catch.hpp"
#include "commands/ls-tree.h"
#include "repository.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"
#include <filesystem>

// TODO: flesh out test cases

TEST_CASE("ls-tree command", "[ls-tree]") {
  GitRepoSetup gitRepoSetup;
  SECTION("ls-tree existing branch", "with HEAD, show head commit") {
    std::vector<std::string> args({"ls-tree", "HEAD"});
    ;

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE_STDOUT_VALUE(commands::lstree(args), "100644 blob [hash]    test\n"
                                                 "100644 blob [hash]    test1");
  }
  SECTION("ls-tree existing commit", "with commit hash") {
    std::vector<std::string> args({"ls-tree", SECOND_COMMIT});
    REQUIRE_STDOUT_VALUE(commands::lstree(args), "100644 blob [hash]    test\n"
                                                 "100644 blob [hash]    test1");
  }
  SECTION("ls-tree with tree", "with tree") {
    std::vector<std::string> args({"ls-tree", SECOND_COMMIT});
    commands::lstree(args);
  }
  SECTION("ls-tree tree with pattern") {
    std::vector<std::string> args({"ls-tree", "-b", "test"});
    commands::lstree(args);

    REQUIRE_STDOUT_VALUE(commands::lstree(args),
                         "100644 blob [hash]    test\n");
  }
}

TEST_CASE("ls-tree with errors", "[ls-tree errors]") {
  GitRepoSetup gitRepoSetup;
  SECTION("branch already exists") {
    std::vector<std::string> args({"ls-tree", "-b", "test_branch"});

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE_THROWS_WITH(commands::lstree(args),
                        "fatal: a branch named 'test_branch' already exists.");
  }
}
