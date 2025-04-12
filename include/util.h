#ifndef UTIL_H
#define UTIL_H

#include <boost/filesystem.hpp>
#include <string>
namespace fs = boost::filesystem;

std::string read_file(const fs::path &filePath);
bool create_file(const fs::path &filePath, const std::string &content = "");
std::string sha1_hexdigest(const std::string &data);
std::string binaryToHex(const std::string &binary);
std::string hexToBinary(const std::string &hexString);
#endif // UTIL_H