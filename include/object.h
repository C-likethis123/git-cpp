#ifndef OBJECT_H
#define OBJECT_H

#include <filesystem>
#include <string>

#include "repository.h"

namespace fs = std::filesystem;
class GitObject {
public:
  GitObject();
  GitObject(const std::string &format);

  virtual void deserialise(const std::string &data) = 0;
  virtual std::string serialise(GitRepository &repo) = 0;

  static std::string write(GitRepository &repo, const std::string &type,
                           const std::string &data, bool write);
  static GitObject *read(GitRepository &repo, const std::string &sha);
  static std::string find(GitRepository &repo, const std::string &name,
                          bool follow = true);

  void init();
  std::string get_type() const;

protected:
  fs::path worktree;
  fs::path gitdir;
  std::string format;
};

#endif // OBJECT_H
