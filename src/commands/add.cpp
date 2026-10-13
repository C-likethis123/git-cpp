#include "commands/add.h"
#include "index.h"
#include "parsers/AddParser.h"
#include "wildmatch/wildmatch.hpp"
#include <filesystem>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;
namespace {
bool add_file(const fs::directory_entry &entry, const fs::path &relative,
              const std::string &pattern, GitRepository &repo,
              std::unordered_set<std::string> &matches) {
  if (!entry.is_regular_file()) {
    return false;
  }
  const std::string file = relative.generic_string();
  if ((pattern == "." || wild::match(pattern, file, wild::LEADING_DIR)) &&
      !repo.is_ignored(entry.path())) {
    matches.insert(file);
    return true;
  }
  return false;
}

bool add_folder(const fs::path &folder, const fs::path &root,
                const std::string &pattern, GitRepository &repo,
                std::unordered_set<std::string> &matches) {
  bool matched = false;
  const std::string parent_pattern = fs::path(pattern).parent_path().generic_string();
  const bool match_files = parent_pattern.empty() ||
      wild::match(parent_pattern, folder.lexically_relative(root).generic_string(),
                  wild::LEADING_DIR);
  for (const auto &entry : fs::directory_iterator(folder)) {
    const fs::path relative = entry.path().lexically_relative(root);
    if (entry.is_directory()) {
      if (!repo.is_ignored(entry.path()) && !entry.is_symlink()) {
        // Wildcards can cross slashes, so deeper levels may still match.
        matched = add_folder(entry.path(), root, pattern, repo, matches) || matched;
      }
    } else if (match_files) {
      matched = add_file(entry, relative, pattern, repo, matches) || matched;
    }
  }
  return matched;
}

bool add_path(const fs::path &relative_pattern, const fs::path &root,
              const std::string &pattern, GitRepository &repo,
              std::unordered_set<std::string> &matches) {
  fs::path folder = root;
  for (const auto &component : relative_pattern) {
    if (component == "." || component.empty()) {
      continue;
    }
    // Ordinary wildcards can cross slashes, so only literal levels can be pruned.
    if (component.generic_string().find_first_of("*?[\\") != std::string::npos) {
      return add_folder(folder, root, pattern, repo, matches);
    }
    const fs::path candidate = folder / component;
    if (!fs::exists(candidate)) {
      return false;
    }
    const fs::directory_entry entry(candidate);
    const fs::path relative = candidate.lexically_relative(root);
    if (!entry.is_directory()) {
      return add_file(entry, relative, pattern, repo, matches);
    }
    if (repo.is_ignored(entry.path()) || entry.is_symlink()) {
      return false;
    }
    folder = candidate;
  }
  // The literal directory already matched; only ignore checks remain.
  return add_folder(folder, root, ".", repo, matches);
}
} // namespace

namespace commands {
void add(std::vector<std::string> &args) {
  AddParser &parser = AddParser::get();
  parser.parse(args);
  const std::vector<std::string> paths = parser.getPaths();
  GitRepository repo = GitRepository::find();
  const fs::path root = fs::canonical(repo.worktree_path("."));

  std::unordered_set<std::string> matches;
  for (const std::string &path : paths) {
    if (!path.empty() && path.front() == ':') {
      throw std::runtime_error("pathspec magic is not supported yet");
    }
    // Pathspecs may contain wildcards, so they cannot be canonicalized.
    const fs::path relative_pattern =
        fs::absolute(path).lexically_normal().lexically_relative(root);
    if (relative_pattern.empty() || *relative_pattern.begin() == "..") {
      throw std::runtime_error("path " + path + " is outside the repository");
    }
    std::string pattern = relative_pattern.generic_string();
    while (pattern.size() > 1 && pattern.back() == '/') {
      pattern.pop_back();
    }
    if (!add_path(relative_pattern, root, pattern, repo, matches)) {
      throw std::runtime_error("path " + path + " did not match any files");
    }
  }

  GitIndex index = GitIndex::read(repo);
  for (const auto &file : matches) {
    index.add_file(file, repo);
  }
  index.save(repo);
}
} // namespace commands
