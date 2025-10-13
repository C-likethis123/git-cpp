#ifndef PACK_INDEX_H
#define PACK_INDEX_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;
class PackIndex {
public:
  PackIndex(const fs::path &path);
  bool has_object(const std::string &sha);
  size_t get_offset(const std::string &sha);

private:
  fs::path path_;
};
#endif // PACK_INDEX_H
