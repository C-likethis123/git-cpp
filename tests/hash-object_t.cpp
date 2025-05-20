#include "catch2/catch.hpp"
#include "commands/hash-object.h"
#include "repository.h"
#include "utils/gitreposetup.h"
#include <filesystem>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

TEST_CASE("hashobject command", "[hashobject]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid git hashobject command - hashobject commit",
          "hashobject commit") {
    std::vector<std::string> args({"cat-file", "commit", "head"});

    std::stringstream buffer;
    std::streambuf *oldCout = std::cout.rdbuf();
    std::cout.rdbuf(buffer.rdbuf());
    commands::hashobject(args);
    std::cout.rdbuf(oldCout);
    REQUIRE(buffer.str() ==
            "tree 19f66fa2b7cba386a1d185449eed1c024d71df25\n"
            "parent 1723ac93b92db1fc2c28de8e5da814136937f8c6\n"
            "author Chow Jia Ying <chowjiaying211@gmail.com> 1747142657 +0800\n"
            "committer Chow Jia Ying <chowjiaying211@gmail.com> 1747142657 "
            "+0800\n\n\n"

            "Test file2\n");
  }
  SECTION("Valid git hashobject command - hashobject a blob",
          "hashobject blob") {
    std::vector<std::string> args(
        {"cat-file", "blob", "9daeafb9864cf43055ae93beb0afd6c7d144bfa4"});
    std::stringstream buffer;
    std::streambuf *oldCout = std::cout.rdbuf();
    std::cout.rdbuf(buffer.rdbuf());
    commands::hashobject(args);
    std::cout.rdbuf(oldCout);
    REQUIRE_NOTHROW(GitRepository(VALID_GIT_PATH, true));
    REQUIRE(buffer.str() == "test\n");
  }
}
