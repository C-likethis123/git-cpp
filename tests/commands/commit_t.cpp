#include "catch2/catch.hpp"
#include "commands/commit.h"
#include "index.h"
#include "object.h"
#include "repo_util.h"
#include "tree.h"
#include "util.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include <iostream>
#include <regex>
#include <sstream>

namespace {
class CommitOutput {
public:
  CommitOutput() : out(std::cout.rdbuf(buffer.rdbuf())),
                   err(std::cerr.rdbuf(errors.rdbuf())) {}
  ~CommitOutput() {
    std::cout.rdbuf(out);
    std::cerr.rdbuf(err);
  }
  std::string hash() const {
    std::smatch match;
    const auto output = buffer.str();
    if (!std::regex_search(output, match, std::regex("\\[([0-9a-f]{40})\\]"))) {
      throw std::runtime_error("commit did not print its object ID");
    }
    return match[1].str();
  }

private:
  std::ostringstream buffer;
  std::ostringstream errors;
  std::streambuf *out;
  std::streambuf *err;
};

void configure_identity(GitRepository &repo) {
  create_file(repo.repo_path("config"), read_file(repo.repo_path("config")) +
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
  index.add_file("nested/staged", repo);
  index.save(repo);
  const auto original_index = read_file(repo.repo_path("index"));
  file_utils::create_file("nested/staged", "unstaged content\n");
  file_utils::create_file("untracked", "untracked content\n");

  std::vector<std::string> args{"commit", "-m", "subject\n\nbody"};
  CommitOutput output;
  commands::commit(args);
  const auto hash = output.hash();
  const auto [type, payload] = read_git_object_data(repo, hash);
  CHECK(type == "commit");
  CHECK(GitObject::write(repo, GitObjectType::Commit, payload, false) == hash);
  CHECK(payload.find("\nparent " + HEAD_COMMIT + "\n") != std::string::npos);
  CHECK(payload.find("\n\nsubject\n\nbody\n") != std::string::npos);
  CHECK(std::regex_search(payload, std::regex(
      "\nauthor Commit Test <commit@example.com> [0-9]+ [+-][0-9]{4}\n")));
  CHECK(std::regex_search(payload, std::regex(
      "\ncommitter Commit Test <commit@example.com> [0-9]+ [+-][0-9]{4}\n")));

  // Inspect the tree independently so malformed identities do not hide staging bugs.
  REQUIRE(payload.substr(0, 5) == "tree ");
  const auto tree = GitTree::read(repo, payload.substr(5, 40));
  REQUIRE(tree.fileEntries.size() == 1);
  const auto nested = GitTree::read(repo, std::get<1>(tree.fileEntries.at("nested")));
  REQUIRE(nested.fileEntries.size() == 1);
  CHECK(read_git_object_data(repo, std::get<1>(nested.fileEntries.at("staged"))).second ==
        "staged content\n");
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
  auto index = GitIndex::read(repo);
  index.add_file("newfile", repo);
  index.save(repo);

  std::vector<std::string> args{"commit", "-m", "new commit"};
  CommitOutput output;
  commands::commit(args);
  CHECK(read_file(repo.repo_path("HEAD")) == head);
  CHECK(GitObject::find(repo, "HEAD") == output.hash());
  CHECK(GitRepoSetup::get_file_contents(repo.branch_path("main")) == output.hash());
}

TEST_CASE("commit creates a root commit on an unborn branch", "[commit-command]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  configure_identity(repo);
  create_file(repo.repo_path("HEAD"), "ref: refs/heads/unborn\n");
  GitIndex index(2, {});
  file_utils::create_file("first", "initial content");
  index.add_file("first", repo);
  index.save(repo);

  std::vector<std::string> args{"commit", "-m", "initial commit"};
  CommitOutput output;
  REQUIRE_NOTHROW(commands::commit(args));
  const auto hash = output.hash();
  CHECK(GitObject::find(repo, "HEAD") == hash);
  const auto payload = read_git_object_data(repo, hash).second;
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
  auto index = GitIndex::read(repo);
  index.add_file("newfile", repo);
  index.save(repo);

  std::vector<std::string> args{"commit", "-m", "detached commit"};
  CommitOutput output;
  commands::commit(args);
  CHECK(GitRepoSetup::get_file_contents(repo.repo_path("HEAD")) == output.hash());
  CHECK(GitRepoSetup::get_file_contents(repo.branch_path("main")) == HEAD_COMMIT);
}

TEST_CASE("commit rejects unresolved conflicts without moving HEAD",
          "[commit-command]") {
  GitRepoSetup setup;
  auto repo = GitRepository::find();
  const auto stage = GENERATE(IndexStage::Base, IndexStage::Ours, IndexStage::Theirs);
  GitIndexEntry entry(0, 0, 0, 0, 0, 0, 0100644, 0, 0, 0, 0, 0, 0,
                      HEAD_COMMIT, false, false, stage, 8, "conflict");
  GitIndex(2, {{"conflict", entry}}).save(repo);
  const auto original_index = read_file(repo.repo_path("index"));
  std::vector<std::string> args{"commit", "-m", "unresolved"};
  CommitOutput output;
  REQUIRE_THROWS_WITH(commands::commit(args),
                      "Cannot write tree: unmerged path conflict");
  CHECK(GitObject::find(repo, "HEAD") == HEAD_COMMIT);
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
  CommitOutput output;
  REQUIRE_THROWS(commands::commit(args));
  CHECK(read_file(repo.repo_path("HEAD")) == head);
  CHECK(GitObject::find(repo, "HEAD") == HEAD_COMMIT);
  CHECK(read_file(repo.repo_path("index")) == index);
}
