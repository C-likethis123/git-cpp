#include "catch2/catch.hpp"
#include "repository.h"
#include "utils/gitreposetup.h"

TEST_CASE("GitRepository Construction", "[GitRepository]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid Git Repository") {
    // Provide a path to a valid git repository
    std::string validGitPath = "test_dir";
    REQUIRE_NOTHROW(GitRepository(validGitPath, true));
  }
}

TEST_CASE("GitRepository Create", "[GitRepository]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Create Git Repository in default path") {
    std::string createGitPath = ".";
    GitRepository repo(createGitPath, true);

    // Use the create function
    REQUIRE_NOTHROW(repo.create(true));

    // Assert that specific files are created
    REQUIRE(fs::exists(".git/branches"));
    REQUIRE(fs::exists(".git/objects"));
    REQUIRE(fs::exists(".git/refs/heads"));
    REQUIRE(fs::exists(".git/refs/tags"));
    REQUIRE(fs::exists(".git/description"));
    REQUIRE(fs::exists(".git/HEAD"));
    REQUIRE(fs::exists(".git/config"));

    // Optionally, you can also check the contents of the files if needed
    // For example:
    // REQUIRE(util::readFile(createGitPath + "description") == "Expected
    // contents");
  }
}

TEST_CASE("GitRepository Ignore", "[GitRepository]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Ignore files in Git Repository") {
    std::string createGitPath = ".";
    GitRepository repo(createGitPath, true);

    REQUIRE_NOTHROW(repo.create(true));

    // Assert that gitignore is respected
    REQUIRE(repo.is_ignored(".git") == true);
    REQUIRE(repo.is_ignored("test2") == false);
    REQUIRE(repo.is_ignored("test3") == true);
  }
}
