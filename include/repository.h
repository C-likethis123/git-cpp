#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;
class GitRepository {
public:
  GitRepository(fs::path worktree, fs::path gitdir);
  GitRepository(const std::string &path, bool force = false);

  static GitRepository find(const fs::path &path = fs::path("."),
                            bool required = true);
  void update_head(const std::string &new_head);

  /**
  Creates all files needed to represent a git directory
  */
  std::string create(bool mkdir);
  fs::path repo_path(fs::path path);
  fs::path worktree_path(fs::path path);
  fs::path dir(const fs::path &path, bool mkdir = false);
  fs::path file(fs::path &gitdir, bool mkdir = false);

  fs::path branch_path(const std::string &branch);
  fs::path object_path(const std::string &sha);

  /* Checks if refs exists */
  bool has_branch(const std::string &branch);
  bool has_object(const std::string &sha);

protected:
  fs::path worktree;
  fs::path gitdir;
};

#endif // REPOSITORY_H
