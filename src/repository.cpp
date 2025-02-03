#include "repository.h"
#include "inih/INIReader.h"
#include "util.h"
#include <iostream>

namespace fs = boost::filesystem;

GitRepository::GitRepository(std::string path, bool force)
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
  create_file(gitdir / "HEAD", "ref: refs/heads/master\n");
  create_file(gitdir / "config", "[core]\n"
                                 "\trepositoryformatversion = 0\n"
                                 "\tfilemode = false\n"
                                 "\tbare = false\n");
  return worktree.string();
}

/*
Computes the path under repo's gitdir
*/
fs::path GitRepository::repo_path(fs::path path) { return gitdir / path; }
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
  if (dir(path.branch_path(), mkdir) != fs::path("")) {
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
    GitRepository repo(path.string());
    return repo;
  }
  fs::path parent = path.parent_path();
  if (parent == path) {
    std::cerr << "No git directory found\n";
    return std::nullopt;
  }
  return find(parent, required);
}
