#ifndef OBJECT_H
#define OBJECT_H

#include <filesystem>
#include <string>

#include "repository.h"

namespace fs = std::filesystem;
enum class GitObjectType { Blob, Tree, Commit };

std::string to_string(GitObjectType type);
GitObjectType git_object_type_from_string(const std::string &type);

class GitObject {
public:
  GitObject();
  explicit GitObject(GitObjectType format);

  virtual void deserialise(const std::string &data) = 0;
  virtual std::string serialise(GitRepository &repo) = 0;

  static std::string write(GitRepository &repo, GitObjectType type,
                           const std::string &data, bool write);
  static GitObject *read(GitRepository &repo, const std::string &sha);
  static std::string find(GitRepository &repo, const std::string &name,
                          bool follow = true);

  void init();
  GitObjectType get_type() const;

protected:
  fs::path worktree;
  fs::path gitdir;
  GitObjectType format = GitObjectType::Blob;
};

#endif // OBJECT_H
