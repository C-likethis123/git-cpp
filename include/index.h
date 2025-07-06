#ifndef INDEX_H
#define INDEX_H

#include "index_entry.h"
#include "repository.h"

class GitIndex {
public:
  GitIndex(uint32_t fileVersion, std::vector<GitIndexEntry> entries);
  static GitIndex read(GitRepository &repo);

  void print_matching_patterns(GitRepository &repo, const std::string &path);

protected:
  uint32_t version_;
  std::vector<GitIndexEntry> entries_;
};

#endif // INDEX_H
