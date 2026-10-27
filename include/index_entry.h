#ifndef INDEX_ENTRY_H
#define INDEX_ENTRY_H

#include "repository.h"
#include <cstdint>
#include <filesystem>
#include <sstream>
#include <string>
namespace fs = std::filesystem;
enum class IndexStage : uint8_t {
  Resolved = 0,
  Base = 1,
  Ours = 2,
  Theirs = 3,
};

class GitIndexEntry {
public:
  GitIndexEntry(uint32_t ctime_sec, uint32_t ctime_nanosec, uint32_t mtime_sec,
                uint32_t mtime_nanosec, uint32_t dev, uint32_t ino,
                uint32_t mode, uint8_t object_type, uint8_t unused,
                uint16_t permissions, uint32_t uid, uint32_t gid,
                uint32_t file_size, std::string sha1, bool flag_assume_valid,
                bool flag_extended, IndexStage flag_stage,
                uint32_t file_name_length, std::string file_name);
  static GitIndexEntry create_index_entry(const fs::path &path,
                                          GitRepository &repo);
  std::string file_name() const;
  uint32_t mtime_sec() const;
  uint32_t file_size() const;
  std::string sha1() const;
  uint32_t mode() const { return mode_; }
  IndexStage stage() const { return flag_stage_; }
  bool is_submodule() const { return mode_ == 0160000; }
  void save(GitRepository &repo, std::stringstream &stream) const;

private:
  uint32_t ctime_sec_;
  uint32_t ctime_nanosec_;
  uint32_t mtime_sec_;
  uint32_t mtime_nanosec_;
  uint32_t dev_;
  uint32_t ino_;
  uint32_t mode_;
  uint8_t object_type_;
  uint8_t unused_;
  uint16_t permissions_;
  uint32_t uid_;
  uint32_t gid_;
  uint32_t file_size_;
  std::string sha1_;
  bool flag_assume_valid_;
  bool flag_extended_;
  IndexStage flag_stage_;
  uint32_t file_name_length_;
  std::string file_name_;
};

#endif // INDEX_ENTRY_H
