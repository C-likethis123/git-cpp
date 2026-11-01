#include "commit.h"
#include "commands/commit.h"
#include "index.h"
#include "inih/INIReader.h"
#include "object.h"
#include "parsers/CommitParser.h"
#include "repository.h"
#include "util.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <print>
#include <stdexcept>
#include <vector>

namespace commands {
void commit(std::vector<std::string> &args) {
  const auto options = CommitParser::parse(args);
  const std::string &message = options.message;
  GitRepository repo = GitRepository::find();

  const std::string tree = GitIndex::read().write_tree();

  const std::string head = read_file(repo.repo_path("HEAD"), true);
  std::vector<std::string> parents;
  try {
    parents.push_back(GitObject::find("HEAD"));
  } catch (const std::runtime_error &error) {
    // Only a missing branch reference represents an unborn HEAD.
    if (!head.starts_with("ref: refs/heads/") ||
        error.what() !=
            "Reference not found: " + repo.repo_path(head.substr(5)).string()) {
      throw;
    }
  }

  // create commit
  const auto curr_time = std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
  std::string name;
  std::string email;
  if (const char *home = std::getenv("HOME")) {
    INIReader conf((fs::path(home) / ".gitconfig").string());
    name = conf.GetString("user", "name", "");
    email = conf.GetString("user", "email", "");
  }
  INIReader local_config(repo.repo_path("config").string());
  name = local_config.GetString("user", "name", name);
  email = local_config.GetString("user", "email", email);

  if (name.empty() || email.empty() ||
      name.find_first_of("\r\n<>") != std::string::npos ||
      email.find_first_of("\r\n<>") != std::string::npos) {
    throw std::runtime_error("Invalid commit name or email");
  }
  // shortcut: use libc for the offset until libc++ supports chrono timezones.
  const auto local =
      std::chrono::zoned_time{std::chrono::current_zone(), curr_time};
  const auto timezone = std::format("{:%z}", local);
  const GitCommit::Identity identity{name, email, curr_time, timezone};
  GitCommit commit(tree, parents, identity, identity, message + "\n");
  const std::string created_hash =
      GitObject::write(GitObjectType::Commit, commit.serialise(), true);
  const std::string short_hash = created_hash.substr(0, 7);
  repo.update_current_ref(created_hash);
  if (repo.has_detached_head()) {
    std::println(std::cerr, "You are in detached head now");
    std::println(std::cout, "[{}] {}", short_hash, message);
  } else {
    std::println(std::cout, "[{} {}] {}", repo.get_branch_name(), short_hash,
                 message);
  }
}
} // namespace commands
