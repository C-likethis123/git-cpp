#include "repository.h"
#include "inih/INIReader.h"
#include "pack_index.h"
#include "util.h"
#include "wildmatch/wildmatch.hpp"
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

GitRepository::GitRepository(const std::string &path, bool force)
    : worktree(fs::path(path)), gitdir(fs::path(path) / ".git") {
  if (!force && !fs::is_directory(gitdir)) {
    std::cerr << "Not a git repository: " << gitdir << "\n";
    // Handle error condition, throw an exception or set some flag, etc.
  }

  fs::path configPath = gitdir / "config";

  if (fs::exists(configPath.string())) {
    INIReader conf(configPath.string());
    int vers = conf.GetInteger("core", "repositoryformatversion", -1);
    if (vers != 0) {
      std::cerr << "Unsupported repositoryformatversion " << vers << "\n";
      // Handle error condition, throw an exception or set some flag, etc.
    }
  }

  ignore_patterns.insert(".git/");
  load_ignore_patterns();
}

void GitRepository::load_ignore_patterns() {
  if (!fs::exists(worktree)) {
    return;
  }
  load_ignore_file({});
  for (auto it = fs::recursive_directory_iterator(worktree);
       it != fs::recursive_directory_iterator(); ++it) {
    const auto &entry = *it;
    if (!entry.is_directory()) {
      continue;
    }
    const fs::path relative = entry.path().lexically_relative(worktree);
    if (entry.is_symlink() || is_ignored(relative, true)) {
      it.disable_recursion_pending();
      continue;
    }
    load_ignore_file(relative);
  }
}

void GitRepository::load_ignore_file(const fs::path &directory) {
  const std::string prefix = directory.generic_string();
  const fs::path ignore_file = worktree / directory / ".gitignore";
  try {
    if (!fs::is_regular_file(ignore_file)) {
      return;
    }
    std::ifstream file(ignore_file);
    if (!file.is_open()) {
      std::cerr << "Unable to open .gitignore file: " << ignore_file
                << std::endl;
      return;
    }
    std::string line;
    while (std::getline(file, line)) {
      boost::trim(line);
      if (line.empty() || line[0] == '#') {
        continue;
      }
      if (!prefix.empty()) {
        const bool directory_only = line.back() == '/';
        if (directory_only) {
          line.pop_back();
        }
        if (line.empty()) {
          continue;
        }
        if (line.front() == '/') {
          line.erase(0, 1);
        } else if (line.find('/') == std::string::npos) {
          line = "**/" + line;
        }
        line = prefix + "/" + line + (directory_only ? "/" : "");
      }
      ignore_patterns.insert(line);
    }
  } catch (const std::exception &e) {
    std::cerr << "Exception reading .gitignore: " << e.what() << std::endl;
  }
}

std::string GitRepository::get_branch_name() {
  const std::string head = read_file(repo_path("HEAD"), true);
  if (head.rfind("ref: refs/heads/", 0) != 0) {
    throw std::runtime_error(
        "Failed to get branch name, repo is in detached HEAD");
  }
  return head.substr(16);
}

/**
Creates all files needed to represent a git directory
*/
std::string GitRepository::create(bool mkdir) {
  if (!fs::exists(worktree)) {
    fs::create_directories(worktree);
  }
  fs::create_directories(gitdir / "branches");
  fs::create_directories(gitdir / "objects");
  fs::create_directories(gitdir / "refs/heads");
  fs::create_directories(gitdir / "refs/tags");

  create_file(gitdir / "description",
              "Unnamed repository; edit this file 'description' to name the "
              "repository.\n");
  create_file(gitdir / "HEAD", "ref: refs/heads/main\n");
  create_file(gitdir / "config", "[core]\n"
                                 "\trepositoryformatversion = 0\n"
                                 "\tfilemode = false\n"
                                 "\tbare = false\n");
  return worktree.string();
}

/*
Computes the path under repo's gitdir - used for files that store the
repository's metadata
*/
fs::path GitRepository::repo_path(fs::path path) { return gitdir / path; }
/*
Computes a path under the worktree - used for files that are tracked in the
repository
*/
fs::path GitRepository::worktree_path(fs::path path) { return worktree / path; }
/*
Creates a directory, or returns None if it is a directory under the git
repository given
*/
fs::path GitRepository::dir(const fs::path &path, bool mkdir) {
  fs::path path_in_repo = repo_path(path);
  if (fs::exists(path_in_repo)) {
    if (fs::is_directory(path_in_repo)) {
      return path_in_repo;
    } else {
      std::cerr << "Not a directory: " << path << "\n";
    }
  }

  if (mkdir) {
    fs::create_directories(path_in_repo);
    return path_in_repo;
  }
  return fs::path("");
}

/**
Returns and optionally creates a path to a file or a directory
*/
fs::path GitRepository::file(fs::path &path, bool mkdir) {
  if (dir(path.parent_path(), mkdir) != fs::path("")) {
    return repo_path(path);
  }
  return fs::path("");
}

/**
Finds root of the current directory
*/
GitRepository GitRepository::find(const fs::path &path, bool required) {
  fs::path canonicalPath = fs::canonical(path);
  fs::path gitDir = canonicalPath / ".git";
  if (fs::is_directory(gitDir)) {
    GitRepository repo(canonicalPath.string());
    return repo;
  }
  fs::path parent = canonicalPath.parent_path();
  if (parent == path) {
    throw std::runtime_error("No git repository found");
  }
  return find(parent, required);
}

void GitRepository::update_head(const std::string &new_head) {
  create_file(gitdir / "HEAD", new_head + "\n");
}

void GitRepository::update_current_ref(const std::string &commit_hash) {
  const std::string head = read_file(repo_path("HEAD"), true);
  const fs::path ref_path = head.rfind("ref: ", 0) == 0
                                ? repo_path(head.substr(5))
                                : repo_path("HEAD");
  if (!create_file(ref_path, commit_hash + "\n")) {
    throw std::runtime_error("Failed to update commit reference: " +
                             ref_path.string());
  }
}

fs::path GitRepository::branch_path(const std::string &branch) {
  return gitdir / "refs/heads" / branch;
}
bool GitRepository::has_branch(const std::string &branch) {
  return fs::exists(branch_path(branch));
}
fs::path GitRepository::object_path(const std::string &sha) {
  std::string dir = sha.substr(0, 2);
  std::string path = sha.substr(2);
  return gitdir / "objects" / dir / path;
}

bool GitRepository::has_loose_object(const std::string &sha) {
  return fs::exists(object_path(sha));
}

bool GitRepository::has_pack_object(const std::string &sha) {
  fs::path pack_dir = gitdir / "objects/pack";
  for (auto &entry : fs::directory_iterator(pack_dir)) {
    if (entry.path().extension() == ".idx") {
      PackIndex idx(entry.path());
      if (idx.has_object(sha)) {
        return true;
      }
    }
  }
  return false;
}
/**
Checks if the file is ignored
Each pattern is a glob file pattern, should parse and match it
according to glob file rules
*/
bool GitRepository::is_ignored(const fs::path &path) const {
  const fs::path relative_path = fs::relative(path, worktree);
  return is_ignored(relative_path, fs::is_directory(fs::symlink_status(path)));
}

bool GitRepository::is_ignored(const fs::path &relative_path,
                               bool directory) const {
  // Checks if the file is nested in a submodule
  for (fs::path parent = directory ? relative_path
                                   : relative_path.parent_path();
       !parent.empty() && parent != "."; parent = parent.parent_path()) {
    if (fs::exists(worktree / parent / ".git")) {
      return true;
    }
  }
  const std::string relative_path_str = relative_path.generic_string();
  const std::string directory_path_str =
      (directory ? relative_path : relative_path.parent_path())
          .generic_string();
  constexpr int flags = wild::WILDSTAR | wild::LEADING_DIR;

  for (auto pattern : ignore_patterns) {
    if (pattern.empty()) {
      continue;
    }
    const bool directory_only = pattern.back() == '/';
    if (directory_only) {
      pattern.pop_back();
    }
    if (pattern.empty()) {
      continue;
    }
    if (pattern.front() == '/') {
      pattern.erase(0, 1);
    } else if (pattern.find('/') == std::string::npos) {
      pattern = "**/" + pattern;
    }
    if (wild::match(pattern,
                    directory_only ? directory_path_str : relative_path_str,
                    flags)) {
      return true;
    }
  }
  return false;
}

std::string GitRepository::get_head() {
  const std::string head_contents = read_file(this->repo_path("HEAD"), true);
  if (head_contents.rfind("ref: ", 0) == 0) {
    std::string ref_contents = head_contents.substr(5);
    if (ref_contents.rfind("refs/heads/", 0) == 0) {
      return ref_contents.substr(11);
    } else if (ref_contents.rfind("refs/tags/", 0) == 0) {
      return ref_contents.substr(10);
    } else {
      return head_contents;
    }
  } else {
    return head_contents;
  }
}
