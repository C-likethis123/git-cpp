#ifndef REPOUTIL_H
#define REPOUTIL_H

#include "repository.h"
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

std::string resolve_ref(const fs::path &ref_path, GitRepository &repo);
std::pair<std::string, std::string>
read_git_object_data(GitRepository &repo, const std::string &sha);
std::pair<std::string, std::string> read_from_pack(const fs::path &pack_path,
                                                   size_t offset);
#endif // REPOUTIL_H
