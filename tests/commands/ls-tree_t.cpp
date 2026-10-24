#include "catch2/catch.hpp"
#include "commands/ls-tree.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"

TEST_CASE("lstree command", "[lstree]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid ls-tree command with tree specified, no file matching pattern "
          "should match everything",
          "ls-tree") {
    std::vector<std::string> args({"ls-tree", HEAD_COMMIT_TREE});
    REQUIRE_STDOUT_VALUE(
        commands::lstree(args),
        "100644 blob df6b0d2bcc76e6ec0fca20c227104a4f28bac41b	.gitignore\n"
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test\n"
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test2\n");
  }

  SECTION("Valid ls-tree command with tree specified, use . should match "
          "everything",
          "ls-tree") {
    std::vector<std::string> args({"ls-tree", HEAD_COMMIT_TREE, "."});
    REQUIRE_STDOUT_VALUE(
        commands::lstree(args),
        "100644 blob df6b0d2bcc76e6ec0fca20c227104a4f28bac41b	.gitignore\n"
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test\n"
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test2\n");
  }

  SECTION("Valid ls-tree command with tree specified, use * should match "
          "everything",
          "ls-tree") {
    std::vector<std::string> args({"ls-tree", HEAD_COMMIT_TREE, "*"});
    REQUIRE_STDOUT_VALUE(
        commands::lstree(args),
        "100644 blob df6b0d2bcc76e6ec0fca20c227104a4f28bac41b	.gitignore\n"
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test\n"
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test2\n");
  }

  SECTION(
      "Valid ls-tree command with tree specified, only show full match entries",
      "ls-tree") {
    std::vector<std::string> args({"ls-tree", HEAD_COMMIT_TREE, "test"});
    REQUIRE_STDOUT_VALUE(
        commands::lstree(args),
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test\n");
  }

  SECTION("Valid ls-tree command with tree specified, globbing should work",
          "ls-tree") {
    std::vector<std::string> args({"ls-tree", HEAD_COMMIT_TREE, "test*"});
    REQUIRE_STDOUT_VALUE(
        commands::lstree(args),
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test\n"
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test2\n");
  }

  SECTION(
      "Valid ls-tree command with tree specified, multiple file patterns work",
      "ls-tree") {
    std::vector<std::string> args(
        {"ls-tree", HEAD_COMMIT_TREE, "test", ".gitignore"});
    REQUIRE_STDOUT_VALUE(
        commands::lstree(args),
        "100644 blob df6b0d2bcc76e6ec0fca20c227104a4f28bac41b	.gitignore\n"
        "100644 blob 9daeafb9864cf43055ae93beb0afd6c7d144bfa4	test\n");
  }
}
