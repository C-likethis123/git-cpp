#include "index_entry.h"

#include "object.h"
#include "repository.h"
#include "util.h"
#include <bitset>
#include <cstdint>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>

namespace fs = std::filesystem;

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

std::string GitIndexEntry::sha1() const { return this->sha1_; }

GitIndexEntry GitIndexEntry::create_index_entry(const fs::path &path,
                                                GitRepository &repo) {
  struct stat file_stat;
  if (stat(path.c_str(), &file_stat) != 0) {
    throw std::runtime_error("Failed to stat file: " + path.string());
  }

  // extract timestamps
  uint32_t ctime_sec = file_stat.st_ctime;

  uint32_t ctime_nanosec = file_stat.st_ctimespec.tv_nsec;
  uint32_t mtime_sec = file_stat.st_mtime;
  uint32_t mtime_nanosec = file_stat.st_mtimespec.tv_nsec;

  // extract mode
  uint32_t mode = file_stat.st_mode;
  uint8_t object_type = (mode << 16) >> 12; // TODO: what is S_IFMT?
  uint8_t unused = mode >> 29;
  uint16_t permissions = mode & 0x1FF;
  uint32_t uid = file_stat.st_uid;
  uint32_t gid = file_stat.st_gid;
  uint32_t file_size = file_stat.st_size;

  // Create blob and sha1
  std::string file_contents = read_file(path);
  std::string sha1 = GitObject::write(repo, "blob", file_contents, false);

  // File name and flags
  std::string file_name = fs::relative(path, repo.worktree_path("")).string();
  uint32_t file_name_length = file_name.size();
  bool flag_assume_valid = false;
  bool flag_extended = false;
  uint8_t flag_stage = 0;

  return GitIndexEntry(ctime_sec, ctime_nanosec, mtime_sec, mtime_nanosec,
                       file_stat.st_dev, file_stat.st_ino, mode, object_type,
                       unused, permissions, uid, gid, file_size, sha1,
                       flag_assume_valid, flag_extended, flag_stage,
                       file_name_length, file_name);
}

void GitIndexEntry::save(GitRepository &repo, std::stringstream &stream) const {
  std::streampos entry_start = stream.tellp();

  // Write timestamps (16 bytes)
  write_uint32_to_bytes(stream, ctime_sec_);
  write_uint32_to_bytes(stream, ctime_nanosec_);
  write_uint32_to_bytes(stream, mtime_sec_);
  write_uint32_to_bytes(stream, mtime_nanosec_);

  // Write dev and ino (8 bytes)
  write_uint32_to_bytes(stream, dev_);
  write_uint32_to_bytes(stream, ino_);

  // Write mode (4 bytes)
  write_uint32_to_bytes(stream, mode_);

  // Write uid and gid (8 bytes)
  write_uint32_to_bytes(stream, uid_);
  write_uint32_to_bytes(stream, gid_);

  // Write file size (4 bytes)
  write_uint32_to_bytes(stream, file_size_);

  // Write SHA1 (20 bytes)
  stream.write(hexToBinary(sha1_).c_str(), 20);

  // Write flags and file name length (2 bytes)
  uint16_t actual_name_length = file_name_.size();
  uint16_t flags_and_length =
      (flag_assume_valid_ ? 0x8000 : 0) | (flag_extended_ ? 0x4000 : 0) |
      (flag_stage_ & 0x3) << 12 | (actual_name_length & 0xFFF);
  write_uint16_to_bytes(stream, flags_and_length);

  // Write file name
  stream.write(file_name_.c_str(), file_name_.size());

  // Pad entry to 8-byte boundary
  std::streampos current_pos = stream.tellp();
  size_t entry_size = static_cast<size_t>(current_pos - entry_start);
  size_t padding = 8 - (entry_size % 8);
  stream.write(std::string(padding, '\0').c_str(), padding);
}
