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

TEST_CASE("log traverses merge parents by discovered commit date", "[log]") {
  GitRepoSetup gitRepoSetup;
  std::vector<std::string> args{"log", TEST_MERGE_COMMIT};
  REQUIRE_STDOUT_VALUE(commands::log(args),
                       "commit 631723968f33afd0dab3fddc038014faff31c680\n"
                       "Merge: ad812d1 ba57088\n"
                       "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                       "Date:   Sat Oct 24 22:59:04 2026 +0800\n\n"
                       "\tMerge test_branch into main for merge fixture\n\n"

                       "commit ad812d1e822f869c41345b1203380be93aba597c\n"
                       "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                       "Date:   Sun Oct 12 10:26:43 2025 +0800\n\n"
                       "\tAdd gitignore\n\n"

                       "commit ba570884af934a79081499203ba81750a945e3c5\n"
                       "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                       "Date:   Thu May 15 21:42:10 2025 +0800\n\n"
                       "\tfirst commit in test_branch\n\n"

                       "commit 6c2c22e7b5b7b1682e3c14668499e84141aca0d4\n"
                       "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                       "Date:   Tue May 13 21:24:17 2025 +0800\n\n"
                       "\tTest file2\n\n"

                       "commit 1723ac93b92db1fc2c28de8e5da814136937f8c6\n"
                       "Author: Chow Jia Ying <chowjiaying211@gmail.com>\n"
                       "Date:   Tue May 13 21:13:41 2025 +0800\n\n"
                       "\tTest file\n\n");
}
