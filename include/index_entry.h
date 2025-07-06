#ifndef INDEX_ENTRY_H
#define INDEX_ENTRY_H

#include <cstdint>
#include <string>

class GitIndexEntry {
public:
  GitIndexEntry(uint32_t ctime_sec, uint32_t ctime_nanosec, uint32_t mtime_sec,
                uint32_t mtime_nanosec, uint32_t dev, uint32_t ino,
                uint32_t mode, uint8_t object_type, uint8_t unused,
                uint16_t permissions, uint32_t uid, uint32_t gid,
                uint32_t file_size, std::string sha1, bool flag_assume_valid,
                bool flag_extended, uint8_t flag_stage,
                uint32_t file_name_length, std::string file_name);
  std::string file_name() const;

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
  uint8_t flag_stage_;
  uint32_t file_name_length_;
  std::string file_name_;
};

#endif // INDEX_ENTRY_H
