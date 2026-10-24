#include "catch2/catch.hpp"
#include "commands/check-ignore.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"
#include <filesystem>
#include <string>
#include <vector>

TEST_CASE("check-ignore command", "[check-ignore]") {
  GitRepoSetup gitRepoSetup;

  SECTION("Prints an ignored path even when the file does not exist") {
    REQUIRE_FALSE(std::filesystem::exists("test3"));
    std::vector<std::string> args{"check-ignore", "test3"};
    REQUIRE_STDOUT_VALUE(commands::checkignore(args), "test3\n");
  }

  SECTION("Prints nothing for non-ignored paths") {
    std::vector<std::string> args{"check-ignore", "test", "missing.txt"};
    REQUIRE_STDOUT_VALUE(commands::checkignore(args), "");
  }

  SECTION("Prints nothing when no paths are supplied") {
    std::vector<std::string> args{"check-ignore"};
    REQUIRE_STDOUT_VALUE(commands::checkignore(args), "");
  }

  SECTION("Filters mixed paths and preserves argument order") {
    REQUIRE(file_utils::create_file(".gitignore", "first.txt\nsecond.txt\n"));
    std::vector<std::string> args{"check-ignore", "second.txt", "test",
                                  "first.txt", "missing.txt"};
    REQUIRE_STDOUT_VALUE(commands::checkignore(args),
                         "second.txt\nfirst.txt\n");
  }

  SECTION("Matches wildcard rules against supplied paths") {
    REQUIRE(file_utils::create_file(".gitignore", "*.log\n"));
    std::vector<std::string> args{"check-ignore", "debug.log", "debug.txt",
                                  "nested/error.log"};
    REQUIRE_STDOUT_VALUE(commands::checkignore(args),
                         "debug.log\nnested/error.log\n");
  }

  SECTION("Matches ignored directories and their contents") {
    REQUIRE(file_utils::create_file(".gitignore", "cache/\n"));
    std::filesystem::create_directory("cache");
    std::vector<std::string> args{"check-ignore", "cache", "cache/item.txt",
                                  "cache-other/item.txt"};
    REQUIRE_STDOUT_VALUE(commands::checkignore(args),
                         "cache\ncache/item.txt\n");
  }

  SECTION("Resolves paths from a nested current directory") {
    std::filesystem::create_directory("nested");
    std::filesystem::current_path("nested");
    std::vector<std::string> args{"check-ignore", "../test3", "../test"};
    REQUIRE_STDOUT_VALUE(commands::checkignore(args), "../test3\n");
  }
}
