#include "catch2/catch.hpp"
#include "commands/add.h"
#include "utils/file_utils.h"
#include "repository.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"
#include <filesystem>
namespace fs = std::filesystem;

// TODO: how do we test this? Do we have injection?
// Maybe we inspect the file?
TEST_CASE("add command", "[add]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Adding a new file", "add") {
      std::vector<std::string> args({"add", "newfile"});
      file_utils::create_file(VALID_GIT_PATH / "newfile");
      commands::add(args);
      REQUIRE_THAT(
          GitRepoSetup::get_file_contents(VALID_GIT_PATH/".git/index"),
          Catch::Matchers::Contains("newfile")
      );
  }

   SECTION("Adding a directory -- files added recursively", "add") {
       std::vector<std::string> args({"add", "newfolder"});
       fs::create_directories(VALID_GIT_PATH / "newfolder");
       file_utils::create_file(VALID_GIT_PATH / "newfolder" / "newfile1");
       file_utils::create_file(VALID_GIT_PATH / "newfolder" / "newfile2");
       commands::add(args);
       REQUIRE_THAT(
           GitRepoSetup::get_file_contents(VALID_GIT_PATH/".git/index"),
           Catch::Matchers::Contains("newfolder/newfile1")
       );
       // REQUIRE_THAT(
       //     GitRepoSetup::get_file_contents(VALID_GIT_PATH/".git/index"),
       //     Catch::Matchers::Contains("newfolder/newfile2")
       // );
   //     "git add folder";
   }

   // SECTION("Adding an ignored file") {
   //     "git add ignored";
   // }

   // SECTION("Adding the .git files should not be allowed") {
   //     "git add .git"
   // }
}

// TEST_CASE("add command errors", "[add]") {
    // GitRepoSetup gitRepoSetup;
    // SECTION("Adding a file that does not exist", "[add]") {
    //     "git add x";
    // }
// }
