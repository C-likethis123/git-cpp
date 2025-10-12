#include "catch2/catch.hpp"
#include "commands/cat-file.h"
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
        "+0800\n\n\n"

        "Add gitignore\n");
  }
  SECTION("Valid git catfile command - catfile a blob", "catfile blob") {
    std::vector<std::string> args(
        {"cat-file", "blob", "9daeafb9864cf43055ae93beb0afd6c7d144bfa4"});
    REQUIRE_STDOUT_VALUE(commands::catfile(args), "test\n");
    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
  }
}

// TEST_CASE("catfile with errors", "[catfile errors]") {
//   GitRepoSetup gitRepoSetup;
//   SECTION("Git catfile already exists") {
//     std::vector<std::string> args({"catfile", "v1.0"});
//     commands::catfile(args);

//     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
//     REQUIRE(fs::exists(".git/refs/tags/v1.0"));
//     REQUIRE(GitRepoSetup::get_file_contents(".git/refs/tags/v1.0") ==
//             SECOND_COMMIT);

//     std::vector<std::string> args2({"tag", "v1.0"});
//     REQUIRE_THROWS_WITH(commands::catfile(args2), "tag 'v1.0' already
//     exists");
//   }
//   SECTION("Git tag failed with non-existent commit") {
//     std::vector<std::string> args({"tag", "v1.0", "nonexistentcommit"});
//     REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));

//     REQUIRE_THROWS_WITH(commands::catfile(args),
//                         "nonexistentcommit: not a valid commit");
//     REQUIRE(!fs::exists(".git/refs/tags/v1.0"));
//   }
// }
