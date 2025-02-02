#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <boost/filesystem.hpp>
#include <iostream>
#include <optional>
#include <string>

namespace fs = boost::filesystem;
class GitRepository {
public:
  GitRepository(fs::path worktree, fs::path gitdir);
  GitRepository(std::string path, bool force = false);

  static std::optional<GitRepository> find(const fs::path &path = fs::path("."),
                                           bool required = true);

  /**
  Creates all files needed to represent a git directory
  */
  std::string create(bool mkdir);
  fs::path repo_path(fs::path path);
  fs::path dir(const fs::path &path, bool mkdir = false);
  fs::path file(fs::path &gitdir, bool mkdir = false);

protected:
  fs::path worktree;
  fs::path gitdir;
};

#endif // REPOSITORY_H
