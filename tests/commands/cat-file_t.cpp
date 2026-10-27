#include "catch2/catch.hpp"
#include "commands/cat-file.h"
#include "parsers/CatfileParser.h"
#include "repository.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"
#include <filesystem>
namespace fs = std::filesystem;

TEST_CASE("catfile command", "[catfile]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid git catfile command - catfile commit", "catfile commit") {
    std::vector<std::string> args({"cat-file", "commit", "head"});
    REQUIRE_STDOUT_VALUE(
        commands::catfile(args),
        "tree 2842c57bba208dfff78c2ce033ccf8f13ffe4d62\n"
        "parent 6c2c22e7b5b7b1682e3c14668499e84141aca0d4\n"
        "author Chow Jia Ying <chowjiaying211@gmail.com> 1760236003 +0800\n"
        "committer Chow Jia Ying <chowjiaying211@gmail.com> 1760236003 "
        "+0800\n\n"

        "Add gitignore\n");
  }
  SECTION("Valid git catfile command - catfile a blob", "catfile blob") {
    std::vector<std::string> args(
        {"cat-file", "blob", "9daeafb9864cf43055ae93beb0afd6c7d144bfa4"});
    REQUIRE_STDOUT_VALUE(commands::catfile(args), "test\n");
    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  }
  SECTION("Valid git catfile command - catfile a tree", "catfile tree") {
      std::vector<std::string> args({"cat-file", "tree", "HEAD"});
      REQUIRE_STDOUT_VALUE(
          commands::catfile(args),
          GitRepoSetup::get_file_contents(OLD_CWD / "fixtures/cat_file_tree.bin")
      );
  }
}

TEST_CASE("catfile with errors", "[catfile errors]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Git catfile failed with non-existent commit") {
    std::vector<std::string> args({"catfile", "commit", "nonexistentcommit"});
    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(!fs::exists(".git/refs/tags/nonexistentcommit"));

    REQUIRE_THROWS_WITH(commands::catfile(args),
                        "nonexistentcommit: not a valid reference");
  }
}

TEST_CASE("catfile parser returns a Git object type", "[catfile][parser]") {
  for (const auto type : {GitObjectType::Blob, GitObjectType::Tree,
                          GitObjectType::Commit}) {
    const auto options =
        CatfileParser::parse({"cat-file", to_string(type), "HEAD"});
    REQUIRE(options.type == type);
    REQUIRE(options.hash == "HEAD");
  }
  REQUIRE_THROWS_WITH(
      CatfileParser::parse({"cat-file", "invalid", "HEAD"}),
      "Unknown type: invalid");
}
