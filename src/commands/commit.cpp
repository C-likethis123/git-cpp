#include "commit.h"
#include "commands/commit.h"
#include "index.h"
#include "inih/INIReader.h"
#include "object.h"
#include "parsers/CommitParser.h"
#include "repository.h"
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

  const std::string tree = GitIndex::read(repo).write_tree(repo);

  const std::string &parent_commit = GitObject::find(repo, "head");

  // create commit
  std::time_t curr_time = std::time(nullptr);
  fs::path config_path = fs::path(std::getenv("HOME")) / ".gitconfig";
  std::string name;
  std::string email;
  if (fs::exists(config_path)) {
    INIReader conf(config_path.string());
    name = conf.GetString("user", "name", "");
    email = conf.GetString("user", "email", "");
  } else {
    std::cout << "does not exist in " << config_path;
  }

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
  GitCommit commit(tree, {parent_commit}, identity, identity, message + "\n");
  const std::string created_hash = GitObject::write(
      repo, GitObjectType::Commit, commit.serialise(repo), true);
  repo.update_current_ref(created_hash);
  std::string branch_name = repo.get_branch_name();
  std::cout << "[" << branch_name << " " << created_hash.substr(0, 7) << "] "
            << message << std::endl;
}
} // namespace commands
