#ifndef REPOUTIL_H
#define REPOUTIL_H

#include "repository.h"
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

std::string resolve_ref(const fs::path &ref_path, GitRepository &repo);
std::pair<std::string, std::string>
read_git_object_data(GitRepository &repo, const std::string &sha);
#endif // REPOUTIL_H
