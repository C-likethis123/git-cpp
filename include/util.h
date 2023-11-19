#ifndef UTIL_H
#define UTIL_H

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

bool create_file(const fs::path &filePath, const std::string& content = "");

#endif // UTIL_H