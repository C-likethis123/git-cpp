#include "commit.h"
#include "commands/commit.h"
#include "index.h"
#include "inih/INIReader.h"
#include "object.h"
#include "parsers/CommitParser.h"
#include "repository.h"
#include "util.h"
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
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
    if (head.rfind("ref: refs/heads/", 0) != 0 ||
        error.what() != "Reference not found: " +
                            repo.repo_path(head.substr(5)).string()) {
      throw;
    }
  }

  // create commit
  std::time_t curr_time = std::time(nullptr);
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
  const std::tm *local_time = std::localtime(&curr_time);
  char timezone[6];
  if (!local_time ||
      std::strftime(timezone, sizeof(timezone), "%z", local_time) != 5) {
    throw std::runtime_error("Could not determine local timezone");
  }
  const GitCommit::Identity identity{name, email, curr_time, timezone};
  GitCommit commit(tree, parents, identity, identity, message + "\n");
  const std::string created_hash =
      GitObject::write(GitObjectType::Commit, commit.serialise(), true);
  const std::string short_hash = created_hash.substr(0, 7);
  repo.update_current_ref(created_hash);
  if (repo.has_detached_head()) {
    std::cerr << "You are in detached head now" << std::endl;
    std::cout << "[" << short_hash << "] " << message << std::endl;
  } else {
    std::cout << "[" << repo.get_branch_name() << " " << short_hash << "] "
              << message << std::endl;
  }
}
} // namespace commands
