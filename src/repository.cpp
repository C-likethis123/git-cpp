#include "repository.h"
#include "inih/INIReader.h"
#include "util.h"
#include <filesystem>
#include <iostream>

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
std::optional<GitRepository> GitRepository::find(const fs::path &path,
                                                 bool required) {
  fs::path canonicalPath = fs::canonical(path);
  fs::path gitDir = canonicalPath / ".git";
  if (fs::is_directory(gitDir)) {
    GitRepository repo(canonicalPath.string());
    return repo;
  }
  fs::path parent = canonicalPath.parent_path();
  if (parent == path) {
    std::cerr << "No git directory found at " << canonicalPath.string() << "\n";
    return std::nullopt;
  }
  return find(parent, required);
}

void GitRepository::update_head(const std::string &new_head) {
  create_file(gitdir / "HEAD", new_head + "\n");
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

bool GitRepository::has_object(const std::string &sha) {
  return fs::exists(object_path(sha));
}
