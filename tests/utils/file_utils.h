#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <filesystem>
#include <string>
namespace fs = std::filesystem;

namespace file_utils {

bool create_file(const fs::path &path, const std::string &content = "");

} // namespace file_utils

#endif
