#ifndef UTIL_H
#define UTIL_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
namespace fs = std::filesystem;
std::string read_bytes(std::ifstream &file_pos, size_t size);
std::string read_file(const fs::path &filePath, bool remove_newline = false);
bool create_file(const fs::path &filePath, const std::string &content = "");
std::string sha1_hexdigest(const std::string &data);
std::string binaryToHex(const std::string &binary);
std::string hexToBinary(const std::string &hexString);
std::string get_file_type(int mode);
fs::perms get_unix_permissions(int mode);
std::string remove_file_prefix(const fs::path &path,
                               const fs::path &repo_prefix);

uint32_t read_uint32_from_bytes(const std::string &s, size_t offset);
void write_uint32_to_bytes(std::stringstream &stream, uint32_t value);
void write_uint16_to_bytes(std::stringstream &stream, uint16_t value);
#endif // UTIL_H
