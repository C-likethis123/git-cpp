#include "catch2/catch.hpp"
#include "commands/commit.h"
#include "index.h"
#include "object.h"
#include "repo_util.h"
#include "tree.h"
#include "util.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"
#include <iostream>
#include <regex>

namespace {
void configure_identity(GitRepository &repo) {
  create_file(
      repo.repo_path("config"),
      read_file(repo.repo_path("config")) +
          "\n[user]\n\tname = Commit Test\n\temail = commit@example.com\n");
}
} // namespace

TEST_CASE("commit writes a valid object from the staged snapshot",
          "[commit-command]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  configure_identity(repo);
  GitIndex index(2, {});
  fs::create_directories("nested");
  file_utils::create_file("nested/staged", "staged content\n");
  index.add_file("nested/staged");
  index.save();
  const auto original_index = read_file(repo.repo_path("index"));
  file_utils::create_file("nested/staged", "unstaged content\n");
  file_utils::create_file("untracked", "untracked content\n");

  std::vector<std::string> args{"commit", "-m", "subject\n\nbody"};
  REQUIRE_STDOUT_VALUE(
      commands::commit(args),
      "[main " + GitObject::find("HEAD").substr(0, 7) + "] subject\n\nbody\n");
  const auto hash = GitObject::find("HEAD");
  const auto [type, payload] = read_git_object_data(hash);
  CHECK(type == "commit");
  CHECK(GitObject::write(GitObjectType::Commit, payload, false) == hash);
  CHECK(payload.find("\nparent " + HEAD_COMMIT + "\n") != std::string::npos);
  CHECK(payload.find("\n\nsubject\n\nbody\n") != std::string::npos);
  CHECK(std::regex_search(
      payload,
      std::regex(
          "\nauthor Commit Test <commit@example.com> [0-9]+ [+-][0-9]{4}\n")));
  CHECK(std::regex_search(
      payload, std::regex("\ncommitter Commit Test <commit@example.com> [0-9]+ "
                          "[+-][0-9]{4}\n")));

  // Inspect the tree independently so malformed identities do not hide staging
  // bugs.
  REQUIRE(payload.substr(0, 5) == "tree ");
  const auto tree = GitTree::read(payload.substr(5, 40));
  REQUIRE(tree.fileEntries.size() == 1);
  const auto nested = GitTree::read(std::get<1>(tree.fileEntries.at("nested")));
  REQUIRE(nested.fileEntries.size() == 1);
  CHECK(read_git_object_data(std::get<1>(nested.fileEntries.at("staged")))
            .second == "staged content\n");
  CHECK(read_file("nested/staged") == "unstaged content\n");
  CHECK(read_file(repo.repo_path("index")) == original_index);
}

TEST_CASE("commit advances the current branch and preserves symbolic HEAD",
          "[commit-command]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  configure_identity(repo);
  const auto head = read_file(repo.repo_path("HEAD"));
  file_utils::create_file("newfile", "new content");
  auto index = GitIndex::read();
  index.add_file("newfile");
  index.save();

  std::vector<std::string> args{"commit", "-m", "new commit"};
  REQUIRE_STDOUT_VALUE(
      commands::commit(args),
      "[main " + GitObject::find("HEAD").substr(0, 7) + "] new commit\n");
  const auto hash = GitObject::find("HEAD");
  CHECK(hash != HEAD_COMMIT);
  CHECK(read_file(repo.repo_path("HEAD")) == head);
  CHECK(GitRepoSetup::get_file_contents(repo.branch_path("main")) == hash);
}

TEST_CASE("commit creates a root commit on an unborn branch",
          "[commit-command]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  configure_identity(repo);
  create_file(repo.repo_path("HEAD"), "ref: refs/heads/unborn\n");
  GitIndex index(2, {});
  file_utils::create_file("first", "initial content");
  index.add_file("first");
  index.save();

  std::vector<std::string> args{"commit", "-m", "initial commit"};
  REQUIRE_STDOUT_VALUE(
      commands::commit(args),
      "[unborn " + GitObject::find("HEAD").substr(0, 7) + "] initial commit\n");
  const auto hash = GitObject::find("HEAD");
  const auto payload = read_git_object_data(hash).second;
  const auto headers_end = payload.find("\n\n");
  REQUIRE(headers_end != std::string::npos);
  CHECK(payload.substr(0, headers_end).find("\nparent ") == std::string::npos);
}

TEST_CASE("commit advances detached HEAD without moving the branch",
          "[commit-command]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  configure_identity(repo);
  create_file(repo.repo_path("HEAD"), HEAD_COMMIT + "\n");
  file_utils::create_file("newfile", "detached content");
  auto index = GitIndex::read();
  index.add_file("newfile");
  index.save();

  std::vector<std::string> args{"commit", "-m", "detached commit"};
  REQUIRE_STDOUT_VALUE(
      commands::commit(args),
      "[" + GitObject::find("HEAD").substr(0, 7) + "] detached commit\n");
  const auto hash = GitObject::find("HEAD");
  CHECK(hash != HEAD_COMMIT);
  CHECK(GitRepoSetup::get_file_contents(repo.repo_path("HEAD")) == hash);
  CHECK(GitRepoSetup::get_file_contents(repo.branch_path("main")) ==
        HEAD_COMMIT);
}

TEST_CASE("commit rejects unresolved conflicts without moving HEAD",
          "[commit-command]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  const auto stage =
      GENERATE(IndexStage::Base, IndexStage::Ours, IndexStage::Theirs);
  GitIndexEntry entry(0, 0, 0, 0, 0, 0, 0100644, 0, 0, 0, 0, 0, 0, HEAD_COMMIT,
                      false, false, stage, 8, "conflict");
  GitIndex(2, {{"conflict", entry}}).save(COMMIT_H);
  const auto original_index = read_file(repo.repo_path("index"));
  std::vector<std::string> args{"commit", "-m", "unresolved"};
  REQUIRE_THROWS_WITH(commands::commit(args),
                      "Cannot write tree: unmerged path conflict");
  CHECK(GitObject::find("HEAD") == HEAD_COMMIT);
  CHECK(read_file(repo.repo_path("index")) == original_index);
}

TEST_CASE("commit rejects invalid arguments without changing repository state",
          "[commit-command]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  auto args = GENERATE(std::vector<std::string>{"commit"},
                       std::vector<std::string>{"commit", "-m"},
                       std::vector<std::string>{"commit", "--unknown"});
  const auto head = read_file(repo.repo_path("HEAD"));
  const auto index = read_file(repo.repo_path("index"));
  REQUIRE_THROWS(commands::commit(args));
  CHECK(read_file(repo.repo_path("HEAD")) == head);
  CHECK(GitObject::find("HEAD") == HEAD_COMMIT);
  CHECK(read_file(repo.repo_path("index")) == index);
}

TEST_CASE("stdout assertion restores the stream when a command throws",
          "[test-helpers]") {
  const auto previous = std::cout.rdbuf();
  const auto throwing_command = [] {
    REQUIRE_STDOUT_VALUE(throw std::runtime_error("command failed"), "");
  };
  REQUIRE_THROWS_WITH(throwing_command(), "command failed");
  const auto restored = std::cout.rdbuf(previous);
  CHECK(restored == previous);
}
