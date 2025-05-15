#include "boost/algorithm/string/trim.hpp"
#include "catch2/catch.hpp"
#include "commands/tag.h"
#include "repository.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// Helper function to read the contents of a file
std::string file_contents(const fs::path &path) {
  std::ifstream file(path);
  if (file.is_open()) {
    std::string contents((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    boost::trim_right(contents);
    file.close();
    return contents;
  } else {
    throw std::runtime_error("Could not open file: " + path.string());
  }
}
const std::string FIRST_COMMIT = "1723ac93b92db1fc2c28de8e5da814136937f8c6";
const std::string SECOND_COMMIT = "6c2c22e7b5b7b1682e3c14668499e84141aca0d4";
const std::string THIRD_COMMIT = "ba570884af934a79081499203ba81750a945e3c5";
const fs::path VALID_GIT_PATH = fs::temp_directory_path() / "gitrepo";
const fs::path OLD_CWD = fs::current_path();
const fs::path TEST_FILE_PATH = OLD_CWD / "gitrepo";

/**
Uses RAII to manage the setup and teardown of a sample Git repo
*/
class GitRepoSetup {
public:
  GitRepoSetup() { setup(); }
  ~GitRepoSetup() { teardown(); }

  void setup() {
    // set up a git repo with an actual commit
    fs::copy(TEST_FILE_PATH, VALID_GIT_PATH, fs::copy_options::recursive);

    fs::current_path(VALID_GIT_PATH);
  }

  void teardown() {
    fs::remove_all(VALID_GIT_PATH);
    fs::current_path(OLD_CWD);
  }
};

TEST_CASE("tag command", "[tag]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid git tag command - tag name only", "tag name only") {
    std::vector<std::string> args({"tag", "v1.0"});
    commands::tag(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(fs::exists(".git/refs/tags/v1.0"));
    REQUIRE(file_contents(".git/refs/tags/v1.0") == SECOND_COMMIT);
  }
  SECTION("Valid git tag command - tag name with commit",
          "tag name with commit") {
    std::vector<std::string> args({"tag", "v1.0", SECOND_COMMIT});
    commands::tag(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(fs::exists(".git/refs/tags/v1.0"));
    REQUIRE(file_contents(".git/refs/tags/v1.0") == SECOND_COMMIT);
  }
}

TEST_CASE("tag with errors", "[tag errors]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Git tag already exists") {
    std::vector<std::string> args({"tag", "v1.0"});
    commands::tag(args);

    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(fs::exists(".git/refs/tags/v1.0"));
    REQUIRE(file_contents(".git/refs/tags/v1.0") == SECOND_COMMIT);

    std::vector<std::string> args2({"tag", "v1.0"});
    REQUIRE_THROWS_WITH(commands::tag(args2), "tag 'v1.0' already exists");
  }
  SECTION("Git tag failed with non-existent commit") {
    std::vector<std::string> args({"tag", "v1.0", "nonexistentcommit"});
    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));

    REQUIRE_THROWS_WITH(commands::tag(args),
                        "nonexistentcommit: not a valid commit");
    REQUIRE(!fs::exists(".git/refs/tags/v1.0"));
  }
}
