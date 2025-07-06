#include "index_entry.h"

#include <cstdint>
#include <string>

GitIndexEntry::GitIndexEntry(uint32_t ctime_sec, uint32_t ctime_nanosec,
                             uint32_t mtime_sec, uint32_t mtime_nanosec,
                             uint32_t dev, uint32_t ino, uint32_t mode,
                             uint8_t object_type, uint8_t unused,
                             uint16_t permissions, uint32_t uid, uint32_t gid,
                             uint32_t file_size, std::string sha1,
                             bool flag_assume_valid, bool flag_extended,
                             uint8_t flag_stage, uint32_t file_name_length,
                             std::string file_name)
    : ctime_sec_(ctime_sec), ctime_nanosec_(ctime_nanosec),
      mtime_sec_(mtime_sec), mtime_nanosec_(mtime_nanosec), dev_(dev),
      ino_(ino), mode_(mode), object_type_(object_type), unused_(unused),
      permissions_(permissions), uid_(uid), gid_(gid), file_size_(file_size),
      sha1_(std::move(sha1)), flag_assume_valid_(flag_assume_valid),
      flag_extended_(flag_extended), flag_stage_(flag_stage),
      file_name_length_(file_name_length), file_name_(std::move(file_name)){};

std::string GitIndexEntry::file_name() const { return this->file_name_; }
