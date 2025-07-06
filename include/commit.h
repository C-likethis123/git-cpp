#ifndef COMMIT_H
#define COMMIT_H

#include <string>
#include <unordered_map>

#include "object.h"
#include "repository.h"

class GitCommit : public GitObject {
public:
  /**
  Assumption here: a GitCommit object will be serialised from an existing Git
  object. The SHA1 hash will be read from the file name.
  */
  GitCommit(const std::string &data = std::string(""),
            const std::string &sha = std::string(""));
  static GitCommit read(GitRepository &repo, const std::string &hash);

  void deserialise(
      const std::string &data) override; // convert string format to data object
  std::string
  serialise(GitRepository &repo) override; // convert this to a string format
  std::string print_commit();
  void init();
  bool has_parent();
  std::string get_parent();
  std::string get_tree();

protected:
  std::unordered_map<std::string, std::string> keyValuePairs;
  std::string sha;
};

#endif // COMMIT_H
