#ifndef OBJECT_H
#define OBJECT_H

#include <boost/filesystem.hpp>
#include <string>

#include "repository.h"

namespace fs = boost::filesystem;
class GitObject {
public:
  GitObject();
  GitObject(const std::string &data);

  virtual void deserialise(std::string &data) = 0;
  virtual std::string serialise(GitRepository &repo) = 0;

  static std::string write(GitRepository &repo, std::string &type,
                           std::string &data, bool write);
  static GitObject *read(GitRepository &repo, const std::string &sha);
  static std::string find(GitRepository &repo, std::string &name,
                          std::string &fmt, bool follow = true);

  void init();

protected:
  fs::path worktree;
  fs::path gitdir;
  std::string format;
};

#endif // OBJECT_H
