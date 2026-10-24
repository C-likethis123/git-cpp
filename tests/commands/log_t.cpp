#include "catch2/catch.hpp"
#include "commands/log.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"

TEST_CASE("log command", "[log]") {
  GitRepoSetup gitRepoSetup;
  SECTION("VALID log command -- no arguments, should show from HEAD commit") {
    std::vector<std::string> args({"log"});
    REQUIRE_STDOUT_VALUE(commands::log(args),
                         "commit ad812d1e822f869c41345b1203380be93aba597c\n"
                         "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                         "Date:   Sun Oct 12 10:26:43 2025 +0800\n\n"
                         "\tAdd gitignore\n\n"

                         "commit 6c2c22e7b5b7b1682e3c14668499e84141aca0d4\n"
                         "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                         "Date:   Tue May 13 21:24:17 2025 +0800\n\n"
                         "\tTest file2\n\n"

                         "commit 1723ac93b92db1fc2c28de8e5da814136937f8c6\n"
                         "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                         "Date:   Tue May 13 21:13:41 2025 +0800\n\n"
                         "\tTest file\n\n");
  }

  SECTION("VALID log command --with specified commit, should show from there") {
    std::vector<std::string> args({"log", SECOND_COMMIT});
    REQUIRE_STDOUT_VALUE(commands::log(args),
                         "commit 6c2c22e7b5b7b1682e3c14668499e84141aca0d4\n"
                         "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                         "Date:   Tue May 13 21:24:17 2025 +0800\n\n"
                         "\tTest file2\n\n"

                         "commit 1723ac93b92db1fc2c28de8e5da814136937f8c6\n"
                         "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                         "Date:   Tue May 13 21:13:41 2025 +0800\n\n"
                         "\tTest file\n\n");
  }
}
