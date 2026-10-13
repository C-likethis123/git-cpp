#include "catch2/catch.hpp"
#include "commands/add.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include <filesystem>
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
  SECTION("wildcard directory components can span multiple levels") {
    std::vector<std::string> args{"add", "sel*/two.cpp"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/two.cpp"));
    REQUIRE(index.find("selection/one.cpp") == std::string::npos);
  }
  SECTION("literal file paths select only that file") {
    std::vector<std::string> args{"add", "selection/one.cpp"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/one.cpp"));
    REQUIRE(index.find("selection/nested/two.cpp") == std::string::npos);
  }
  SECTION("literal directories select descendants without searching siblings") {
    fs::create_directories(VALID_GIT_PATH / "unrelated");
    file_utils::create_file(VALID_GIT_PATH / "unrelated/.gitignore", "*.cpp\n");
    file_utils::create_file(VALID_GIT_PATH / "selection/.gitignore", "/skip.txt\n");
    std::vector<std::string> args{"add", "selection"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/one.cpp"));
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/two.cpp"));
    REQUIRE(index.find("selection/skip.txt") == std::string::npos);
    REQUIRE(index.find("unrelated/.gitignore") == std::string::npos);
  }
  SECTION("file matching starts at a matching directory level") {
    fs::create_directories(VALID_GIT_PATH / "selection/nested/deeper");
    file_utils::create_file(VALID_GIT_PATH / "selection/two.cpp", "skip");
    file_utils::create_file(VALID_GIT_PATH / "selection/nested/deeper/two.cpp", "deep");
    std::vector<std::string> args{"add", "selection/nest*/two.cpp"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/two.cpp"));
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/deeper/two.cpp"));
    REQUIRE(index.find("selection/two.cpp") == std::string::npos);
  }
  SECTION("unmatched directory levels can contain matching descendants") {
    fs::create_directories(VALID_GIT_PATH / "selection/other/nested");
    file_utils::create_file(VALID_GIT_PATH / "selection/other/nested/two.cpp", "deep");
    std::vector<std::string> args{"add", "selection/*nested/two.cpp"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/other/nested/two.cpp"));
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/two.cpp"));
    REQUIRE(index.find("selection/one.cpp") == std::string::npos);
  }
  SECTION("multiple paths and overlapping folders are supported") {
    std::vector<std::string> args{"add", "selection/one.cpp",
                                  "selection/nested", "selection/nested/two.cpp",
                                  "selection/nested"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/one.cpp"));
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/nested/two.cpp"));
    REQUIRE(index.find("selection/skip.txt") == std::string::npos);
  }
  SECTION("an unmatched later path leaves the index unchanged") {
    const auto before = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    std::vector<std::string> args{"add", "selection/one.cpp", "xxxxxx"};
    REQUIRE_THROWS_WITH(commands::add(args), "path xxxxxx did not match any files");
    REQUIRE(GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index") == before);
  }
  SECTION("literal paths cannot bypass an ignored parent directory") {
    file_utils::create_file(VALID_GIT_PATH / ".gitignore", "selection/\n");
    std::vector<std::string> args{"add", "selection/one.cpp"};
    REQUIRE_THROWS_WITH(commands::add(args),
                        "path selection/one.cpp did not match any files");
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
    REQUIRE_THROWS_WITH(commands::add(args),
                        "path selection/*.missing did not match any files");
    REQUIRE(GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index") == before);
  }
  SECTION("unmatched patterns are rejected before reading the index") {
    file_utils::create_file(VALID_GIT_PATH / ".git/index", "invalid index");
    const std::string pattern = GENERATE("xxxxxx", "selection/*.missing");
    std::vector<std::string> args{"add", pattern};
    REQUIRE_THROWS_WITH(commands::add(args),
                        "path " + pattern + " did not match any files");
    REQUIRE(GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index") ==
            "invalid index");
  }
  SECTION("directory-only ignore patterns prune matching directories") {
    file_utils::create_file(VALID_GIT_PATH / ".gitignore", "selection/nested/\n");
    std::vector<std::string> args{"add", "selection/*"};
    commands::add(args);
    const auto index = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    REQUIRE_THAT(index, Catch::Matchers::Contains("selection/one.cpp"));
    REQUIRE(index.find("selection/nested/two.cpp") == std::string::npos);
  }
  SECTION("symlinks retain target-based ignore checks") {
    fs::create_symlink("one.cpp", VALID_GIT_PATH / "selection/link.cpp");
    file_utils::create_file(VALID_GIT_PATH / ".gitignore", "selection/one.cpp\n");
    const auto before = GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index");
    std::vector<std::string> args{"add", "selection/link.cpp"};
    REQUIRE_THROWS_WITH(commands::add(args),
                        "path selection/link.cpp did not match any files");
    REQUIRE(GitRepoSetup::get_file_contents(VALID_GIT_PATH / ".git/index") == before);
  }
}
