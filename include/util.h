#ifndef UTIL_H
#define UTIL_H

#include "repository.h"
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

std::string read_file(const fs::path &filePath, bool remove_newline = false);
bool create_file(const fs::path &filePath, const std::string &content = "");
std::string sha1_hexdigest(const std::string &data);
std::string binaryToHex(const std::string &binary);
std::string hexToBinary(const std::string &hexString);
std::string get_file_type(int mode);
fs::perms get_unix_permissions(int mode);
std::string remove_file_prefix(const fs::path &path,
                               const fs::path &repo_prefix);
std::string resolve_ref(const fs::path &ref_path, GitRepository &repo);
std::pair<std::string, std::string>
read_git_object_data(GitRepository &repo, const std::string &sha);
uint32_t read_uint32_from_bytes(const std::string &s, size_t offset);
#endif // UTIL_H
