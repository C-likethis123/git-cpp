#ifndef COMMIT_H
#define COMMIT_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

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
  bool has_parents();
  const std::vector<std::string> &get_parents() const;
  std::string get_tree();
  std::int64_t get_committer_timestamp() const;

protected:
  struct Identity {
    std::string name;
    std::string email;
    std::int64_t unix_timestamp;
    std::string timezone;
  };

  std::string tree;
  Identity author;
  Identity committer;
  std::optional<std::string> gpgsig;
  std::string message;
  std::string sha;
  std::vector<std::string> parents;
};

#endif // COMMIT_H
