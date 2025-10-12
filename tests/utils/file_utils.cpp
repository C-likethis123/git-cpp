#include "file_utils.h"
#include <fstream>

namespace fs = std::filesystem;

namespace file_utils {

bool create_file(const fs::path &path, const std::string &content) {
  std::ofstream file(path.string());
  if (file.is_open()) {
    file << content;
    file.close();
    return true;
  }
  return false;
}

} // namespace file_utils
