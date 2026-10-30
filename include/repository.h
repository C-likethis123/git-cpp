#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <filesystem>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;
class GitRepository {
public:
  GitRepository(fs::path worktree, fs::path gitdir);
  GitRepository(const std::string &path, bool force = false);

  static GitRepository find(const fs::path &path = fs::path("."),
                            bool required = true);
  void update_head(const std::string &new_head);
  // Advance the current branch, or HEAD itself when detached.
  void update_current_ref(const std::string &commit_hash);

  /**
  Creates all files needed to represent a git directory
  */
  std::string create(bool mkdir);
  fs::path repo_path(fs::path path) const;
  fs::path worktree_path(fs::path path) const;
  std::string get_head();
  std::string get_branch_name();
  fs::path dir(const fs::path &path, bool mkdir = false);
  fs::path file(fs::path &gitdir, bool mkdir = false);

  fs::path branch_path(const std::string &branch) const;
  fs::path object_path(const std::string &sha) const;
  fs::path tag_path(const std::string &tag) const;

  /* Checks if refs exists */
  bool has_branch(const std::string &branch) const;
  bool has_tag(const std::string &tag) const;
  bool has_loose_object(const std::string &sha) const;
  bool has_pack_object(const std::string &sha) const;
  bool has_detached_head() const;
  /**
  Checks if the file is ignored
  */
  bool is_ignored(const fs::path &path) const;
  // For callers that already know the worktree-relative path and file type.
  bool is_ignored(const fs::path &relative_path, bool directory) const;

protected:
  fs::path worktree;
  fs::path gitdir;
  void load_ignore_patterns();
  void load_ignore_file(const fs::path &directory);
  std::unordered_set<std::string> ignore_patterns{".git"};
};

#endif // REPOSITORY_H
