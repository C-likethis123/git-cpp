#include "pack_index.h"
#include <filesystem>

namespace fs = std::filesystem;
PackIndex::PackIndex(const fs::path &path) : path_(path) {}

bool PackIndex::has_object(const std::string &sha) {
  return fs::exists(path_ / sha);
}

size_t PackIndex::get_offset(const std::string &sha) {
  return fs::file_size(path_ / sha);
}
