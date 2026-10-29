#ifndef INDEX_H
#define INDEX_H

#include "index_entry.h"
#include <string>
#include <unordered_map>

class GitIndex {
public:
  GitIndex(uint32_t fileVersion,
           std::unordered_map<std::string, GitIndexEntry> entries);
  static GitIndex read();

  void print_matching_patterns(const std::string &path);
  void scan_status();

  void add_file(const std::string &path);
  void save();
  std::string write_tree() const;

protected:
  uint32_t version_;
  std::unordered_map<std::string, GitIndexEntry> entries_;

  // void find_modified_unstaged(
  //                             std::vector<std::string> &modified,
  //                             std::vector<std::string> &deleted);
  // std::vector<std::string> find_untracked_files();
};

#endif // INDEX_H
