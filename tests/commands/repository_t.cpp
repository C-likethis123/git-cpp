#include "catch2/catch.hpp"
#include "repository.h"
#include "utils/gitreposetup.h"
#include <fstream>

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
  }

  SECTION("Create Git repository in specified path") {
      std::string createGitPath = "./brandnew";
      GitRepository repo(createGitPath, true);

      // Use the create function
      REQUIRE_NOTHROW(repo.create(true));

      // Assert that specific files are created
      REQUIRE(fs::exists("brandnew/.git/branches"));
      REQUIRE(fs::exists("brandnew/.git/description"));
      REQUIRE(fs::exists("brandnew/.git/HEAD"));
      REQUIRE(fs::exists("brandnew/.git/config"));
      REQUIRE(fs::exists("brandnew/.git/objects"));
      REQUIRE(fs::exists("brandnew/.git/refs/heads"));
      REQUIRE(fs::exists("brandnew/.git/refs/tags"));
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

TEST_CASE("GitRepository ignore glob rules", "[GitRepository]") {
  GitRepoSetup gitRepoSetup;
  std::ofstream(".gitignore") << "*.o\ncache/\n/root.txt\nsrc/generated/\n";
  fs::create_directories("nested/cache");
  fs::create_directories("src/generated");
  fs::create_directories("nested/src/generated");
  fs::create_directories("other");
  std::ofstream("other/cache") << "file, not directory";
  GitRepository repo(".", true);

  REQUIRE(repo.is_ignored(".git/config"));
  REQUIRE(repo.is_ignored("nested/file.o"));
  REQUIRE_FALSE(repo.is_ignored("nested/file.cpp"));
  REQUIRE(repo.is_ignored("nested/cache"));
  REQUIRE(repo.is_ignored("nested/cache/file.txt"));
  REQUIRE_FALSE(repo.is_ignored("other/cache"));
  REQUIRE_FALSE(repo.is_ignored("nested/cache-other/file.txt"));
  REQUIRE(repo.is_ignored("root.txt"));
  REQUIRE_FALSE(repo.is_ignored("nested/root.txt"));
  REQUIRE(repo.is_ignored("src/generated/file.txt"));
  REQUIRE_FALSE(repo.is_ignored("nested/src/generated/file.txt"));
}

TEST_CASE("GitRepository collects nested ignore rules before matching", "[GitRepository]") {
  GitRepoSetup gitRepoSetup;
  fs::create_directories("selected/nested");
  fs::create_directories("unrelated");
  std::ofstream("selected/.gitignore") << "*.cpp\n/cache/\n";
  std::ofstream("unrelated/.gitignore") << "*.txt\n";
  fs::create_directories("skipped");
  std::ofstream(".gitignore") << "skipped/\n";
  std::ofstream("skipped/.gitignore") << "*.secret\n";
  const GitRepository repo(".", true);
  const int collected_patterns = repo.num_ignored_patterns();
  REQUIRE(collected_patterns == 6);
  REQUIRE_FALSE(repo.is_ignored("file.cpp", false));
  REQUIRE(repo.num_ignored_patterns() == collected_patterns);
  REQUIRE(repo.is_ignored("selected/nested/file.cpp", false));
  REQUIRE(repo.is_ignored("selected/cache", true));
  REQUIRE_FALSE(repo.is_ignored("selected/nested/cache", true));
  REQUIRE_FALSE(repo.is_ignored("selected/file.txt", false));
  REQUIRE(repo.num_ignored_patterns() == collected_patterns);
  REQUIRE(repo.is_ignored("unrelated/file.txt", false));
  REQUIRE_FALSE(repo.is_ignored("other/file.txt", false));
  REQUIRE(repo.num_ignored_patterns() == collected_patterns);
}
