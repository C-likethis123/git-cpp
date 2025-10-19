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

  // TODO: read .gitignore files in nested directories.
  // read gitignore here
  fs::path gitignorePath = worktree / ".gitignore";
  if (fs::exists(gitignorePath)) {
    try {
      std::ifstream file(gitignorePath.string());

      if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
          boost::trim(line);
          if (!line.empty() && line[0] != '#') {
            ignore_patterns.insert(line);

            // For directory patterns ending with '/', also add the pattern with
            // '**' appended This ensures that .cache/ matches any file or
            // directory under .cache/
            if (line.back() == '/') {
              std::string expanded_pattern = line + "**";
              ignore_patterns.insert(expanded_pattern);
            }
          }
        }
        file.close();
      } else {
        std::cerr << "Unable to open .gitignore file: " << gitignorePath
                  << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Exception reading .gitignore: " << e.what() << std::endl;
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
bool GitRepository::is_ignored(const fs::path &path) {
  fs::path relative_path = fs::relative(path, worktree);
  std::string relative_path_str = relative_path.generic_string();

  // Use different flags for different pattern types
  constexpr int pathname_flags = wild::PATHNAME | wild::PERIOD |
                                 wild::CASEFOLD | wild::LEADING_DIR |
                                 wild::WILDSTAR;
  constexpr int no_pathname_flags =
      wild::PERIOD | wild::CASEFOLD | wild::LEADING_DIR | wild::WILDSTAR;

  for (const auto &pattern : ignore_patterns) {
    // For patterns that contain path separators or end with '/', use PATHNAME
    // flags For simple patterns like *.a, use no PATHNAME flags to match
    // anywhere
    bool use_pathname =
        (pattern.find('/') != std::string::npos) || pattern.back() == '/';
    int flags = use_pathname ? pathname_flags : no_pathname_flags;

    // Special handling for directory patterns ending with '/'
    // In Git, .cache/ should match .cache/ directories anywhere in the
    // repository
    if (pattern.back() == '/' && !pattern.empty()) {
      // Try matching the pattern as-is first
      if (wild::match(pattern, relative_path_str, flags)) {
        return true;
      }
      // Also try matching with **/ prefix and ** suffix to match anywhere in
      // the repository
      std::string anywhere_pattern = "**/" + pattern + "**";
      if (wild::match(anywhere_pattern, relative_path_str, flags)) {
        return true;
      }
    } else {
      // Normal pattern matching
      if (wild::match(pattern, relative_path_str, flags)) {
        return true;
      }
      // For simple patterns, also try matching anywhere in the repository
      if (!use_pathname) {
        std::string anywhere_pattern = "**/" + pattern;
        if (wild::match(anywhere_pattern, relative_path_str, pathname_flags)) {
          return true;
        }
      }
    }
  }
  return false;
}

int GitRepository::num_ignored_patterns() { return ignore_patterns.size(); }

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
