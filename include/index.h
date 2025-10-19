#ifndef INDEX_H
#define INDEX_H

#include "index_entry.h"
#include "repository.h"
#include <string>
#include <unordered_map>

class GitIndex {
public:
  GitIndex(uint32_t fileVersion,
           std::unordered_map<std::string, GitIndexEntry> entries);
  static GitIndex read(GitRepository &repo);

  void print_matching_patterns(GitRepository &repo, const std::string &path);
  void scan_status(GitRepository &repo);

  void add_file(const std::string &path, GitRepository &repo);
  void save(GitRepository &repo);

protected:
  uint32_t version_;
  std::unordered_map<std::string, GitIndexEntry> entries_;

  // void find_modified_unstaged(GitRepository &repo,
  //                             std::vector<std::string> &modified,
  //                             std::vector<std::string> &deleted);
  // std::vector<std::string> find_untracked_files(GitRepository &repo);
};

#endif // INDEX_H
