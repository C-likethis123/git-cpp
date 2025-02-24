#ifndef COMMIT_H
#define COMMIT_H

#include <string>
#include <unordered_map>

#include "object.h"

class GitCommit : public GitObject {
public:
  GitCommit(const std::string &data = std::string(""));

  void deserialise(
      const std::string &data) override; // convert string format to data object
  std::string
  serialise(GitRepository &repo) override; // convert this to a string format
  std::string print_commit(GitRepository &repo);
  void init();
  bool has_parent();
  std::string get_parent();

protected:
  std::unordered_map<std::string, std::string> keyValuePairs;
};

#endif // COMMIT_H
