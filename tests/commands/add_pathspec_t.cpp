#include "catch2/catch.hpp"
#include "commands/add.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include <filesystem>
#include <iostream>
#include <sstream>
namespace fs = std::filesystem;

TEST_CASE("add pathspec selection", "[add]") {
  GitRepoSetup setup;
  fs::create_directories(VALID_GIT_PATH / "selection/nested");
  file_utils::create_file(VALID_GIT_PATH / "selection/one.cpp", "one");
  file_utils::create_file(VALID_GIT_PATH / "selection/nested/two.cpp", "two");
  file_utils::create_file(VALID_GIT_PATH / "selection/skip.txt", "skip");
  SECTION("wildcards cross directory separators") {
    std::vector<std::string> args{"add", "selection/*.cpp"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/one.cpp"));
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/two.cpp"));
    REQUIRE(index.find("selection/skip.txt") == std::string::npos);
  }
  SECTION("directory with trailing slash selects descendants") {
    std::vector<std::string> args{"add", "selection/"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/two.cpp"));
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/skip.txt"));
  }
  SECTION("subdirectory paths resolve against caller") {
    fs::current_path(VALID_GIT_PATH / "selection");
    std::vector<std::string> args{"add", "nested/"};
    commands::add(args);
    fs::current_path(VALID_GIT_PATH);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/two.cpp"));
    REQUIRE(index.find("selection/one.cpp") == std::string::npos);
  }
  SECTION("unmatched path leaves index unchanged") {
    const auto before = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    std::vector<std::string> args{"add", "selection/*.missing"};
    std::ostringstream error;
    auto *old = std::cerr.rdbuf(error.rdbuf());
    commands::add(args);
    std::cerr.rdbuf(old);
    REQUIRE_THAT(error.str(), Catch::Matchers::Contains("did not match any files"));
    REQUIRE(GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index") == before);
  }
}
