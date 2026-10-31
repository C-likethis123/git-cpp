#include "commit.h"
#include "date_util.h"
#include "repo_util.h"
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

GitCommit::GitCommit(const std::string &data, const std::string &sha)
    : GitObject(GitObjectType::Commit) {
  this->deserialise(data);
  this->sha = sha;
};

GitCommit::GitCommit(const std::string &tree,
                     const std::vector<std::string> &parents,
                     const Identity &author, const Identity &committer,
                     const std::string &message)
    : GitObject(GitObjectType::Commit), tree(tree), author(author),
      committer(committer), message(message), parents(parents) {}

std::string GitCommit::serialise() {
  std::stringstream ss;
  ss << "tree " << tree << "\n";
  for (const auto &parent : parents) {
    ss << "parent " << parent << "\n";
  }
  ss << "author " << author.name << " <" << author.email << "> "
     << author.unix_timestamp << " " << author.timezone << "\n";
  ss << "committer " << committer.name << " <" << committer.email << "> "
     << committer.unix_timestamp << " " << committer.timezone << "\n";
  if (gpgsig) {
    ss << "gpgsig " << *gpgsig << "\n";
  }
  ss << "\n";
  ss << message;
  return ss.str();
}

std::string GitCommit::print_commit() {
  std::stringstream ss;
  ss << "commit " << sha << "\n";
  if (parents.size() > 1) {
    ss << "Merge:";
    for (const auto &parent : parents) {
      ss << " " << parent.substr(0, 7);
    }
    ss << "\n";
  }
  ss << std::left << std::setw(8) << "Author: " << author.name << " <"
     << author.email << ">\n";
  ss << std::setw(8) << "Date: " << format_unix_time(author.unix_timestamp)
     << "\n\n";
  ss << "\t" << message;
  return ss.str();
}

void GitCommit::deserialise(const std::string &data) {
  tree.clear();
  author = {};
  committer = {};
  gpgsig.reset();
  message.clear();
  parents.clear();
  if (data.empty()) {
    return;
  }

  const auto separator = data.find("\n\n");
  if (separator == std::string::npos) {
    throw std::runtime_error(
        "Invalid commit: missing header/message separator");
  }
  message = data.substr(separator + 2);

  std::istringstream headers(data.substr(0, separator));
  std::string line;
  std::string key;
  std::vector<std::pair<std::string, std::string>> fields;
  while (std::getline(headers, line)) {
    if (line.empty()) {
      throw std::runtime_error("Invalid commit: empty header");
    }
    // Multiline headers, such as gpgsig, use space-prefixed continuations.
    if (line.front() == ' ') {
      if (fields.empty() || key == "parent" || key == "tree" ||
          key == "author" || key == "committer") {
        throw std::runtime_error(
            "Invalid commit: unexpected continuation line");
      }
      fields.back().second += "\n" + line;
      continue;
    }
    const auto space = line.find(' ');
    if (space == std::string::npos) {
      throw std::runtime_error("Invalid commit header");
    }
    key = line.substr(0, space);
    const auto value = line.substr(space + 1);
    fields.emplace_back(key, value);
  }

  bool has_tree = false;
  bool has_author = false;
  bool has_committer = false;
  for (const auto &[key, value] : fields) {
    if (key == "parent") {
      if (value.empty()) {
        throw std::runtime_error("Invalid commit: empty parent");
      }
      parents.push_back(value);
      continue;
    }
    if (key == "tree") {
      if (has_tree || value.empty()) {
        throw std::runtime_error("Invalid commit: duplicate or empty tree");
      }
      has_tree = true;
      tree = value;
    } else if (key == "gpgsig") {
      if (gpgsig) {
        throw std::runtime_error("Invalid commit: duplicate gpgsig");
      }
      gpgsig = value;
    } else if (key == "author" || key == "committer") {
      auto &seen = key == "author" ? has_author : has_committer;
      if (seen) {
        throw std::runtime_error("Invalid commit: duplicate " + key);
      }
      seen = true;
      auto &identity = key == "author" ? author : committer;
      const auto email_start = value.rfind(" <");
      const auto email_end = value.find("> ", email_start);
      const auto timestamp_end = email_end == std::string::npos
                                     ? std::string::npos
                                     : value.find(' ', email_end + 2);
      if (email_start == std::string::npos || email_end == std::string::npos ||
          timestamp_end == std::string::npos) {
        throw std::runtime_error("Invalid commit identity: " + key);
      }
      identity.name = value.substr(0, email_start);
      identity.email =
          value.substr(email_start + 2, email_end - email_start - 2);
      const auto timestamp =
          value.substr(email_end + 2, timestamp_end - email_end - 2);
      try {
        std::size_t consumed = 0;
        identity.unix_timestamp = std::stoll(timestamp, &consumed);
        if (consumed != timestamp.size() ||
            timestamp.find_first_not_of("-0123456789") != std::string::npos) {
          throw std::invalid_argument("Invalid timestamp");
        }
      } catch (const std::invalid_argument &) {
        throw std::runtime_error("Invalid commit timestamp: " + key);
      } catch (const std::out_of_range &) {
        throw std::runtime_error("Invalid commit timestamp: " + key);
      }
      identity.timezone = value.substr(timestamp_end + 1);
    }
  }
  if (!has_tree || !has_author || !has_committer) {
    throw std::runtime_error(
        "Invalid commit: missing tree, author or committer");
  }
}

GitCommit GitCommit::read(const std::string &hash) {
  const std::string &sha = GitObject::find(hash);
  auto [fmt, payload] = read_git_object_data(sha);
  if (fmt != "commit") {
    throw std::runtime_error("Invalid commit object: " + hash);
  }
  return GitCommit(payload, sha);
}

std::string GitCommit::get_sha() const { return sha; }
bool GitCommit::has_parents() const { return !parents.empty(); }

const std::vector<std::string> &GitCommit::get_parents() const {
  return parents;
}

std::string GitCommit::get_tree() const { return tree; }

std::int64_t GitCommit::get_committer_timestamp() const {
  return committer.unix_timestamp;
}

bool GitCommit::has_ancestor(const GitCommit &commit) const {
  if (sha.empty() || commit.sha.empty()) {
    throw std::runtime_error("Ancestry requires commits with object IDs");
  }
  if (sha == commit.sha) {
    return true;
  }

  std::vector<std::string> pending = parents;
  std::unordered_set<std::string> visited{sha};
  while (!pending.empty()) {
    const auto hash = pending.back();
    pending.pop_back();
    if (!visited.insert(hash).second) {
      continue;
    }
    if (hash == commit.sha) {
      return true;
    }
    const auto parent = GitCommit::read(hash);
    pending.insert(pending.end(), parent.parents.begin(), parent.parents.end());
  }
  return false;
}
