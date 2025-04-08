#include "commit.h"
#include "repository.h"
#include <sstream>
GitCommit::GitCommit(const std::string &data) : GitObject() {
  this->deserialise(data);
  this->sha = sha;
};

std::string GitCommit::serialise(GitRepository &repo) {
  std::stringstream ss;
  ss << "tree " << this->keyValuePairs["tree"] << "\n";
  if (this->has_parent()) {
    ss << "parent " << this->keyValuePairs["parent"] << "\n";
  }
  ss << "author " << this->keyValuePairs["author_name"] << " <"
     << this->keyValuePairs["author_email"] << "> "
     << this->keyValuePairs["author_unix_timestamp"] << " "
     << this->keyValuePairs["author_timezone"] << "\n";
  ss << "committer " << this->keyValuePairs["committer_name"] << " "
     << this->keyValuePairs["committer_email"] << "> "
     << this->keyValuePairs["committer_unix_timestamp"] << " "
     << this->keyValuePairs["committer_timezone"] << "\n";
  ss << "gpgsig " << this->keyValuePairs["gpgsig"] << "\n\n";
  ss << this->keyValuePairs["message"];
  return ss.str();
}

std::string GitCommit::print_commit(GitRepository &repo) {
  std::stringstream ss;
  ss << "commit " << sha << "\n";
  ss << "Author: " << this->keyValuePairs["author_name"] << " <"
     << this->keyValuePairs["author_email"] << ">\n";
  ss << "Date: " << this->keyValuePairs["author_unix_timestamp"] << " "
     << this->keyValuePairs["author_timezone"] << "\n\n";
  ss << "    " << this->keyValuePairs["message"];
  return ss.str();
}

void GitCommit::deserialise(const std::string &data) {
  const std::unordered_map<std::string, std::string> keyValuePairs;
  int pos = 0;

  const std::array<std::string, 5> keys = {"tree", "parent", "author",
                                           "committer", "gpgsig"};
  const std::vector<std::pair<std::string, std::string>> sub_keys = {
      {"name", " <"},
      {"email", "> "},
      {"unix_timestamp", " "},
      {"timezone", "\n"}};
  for (auto &key : keys) {
    int space = data.find(key + " ", pos);
    if (space == std::string::npos) {
      continue;
    }
    int nl = data.find("\n", space);
    if (key == "author" || key == "committer") {
      pos = space + key.size() + 1;
      for (auto &sub_key : sub_keys) {
        space = data.find(sub_key.second, pos);
        this->keyValuePairs[key + "_" + sub_key.first] =
            data.substr(pos, space - pos);
        pos = space + sub_key.second.size();
      }
      pos = nl + 1;
    } else if (key == "gpgsig") {
      std::string gpgsig_end = "-----END PGP SIGNATURE-----";
      nl = data.find(gpgsig_end, space) + gpgsig_end.size();
      this->keyValuePairs[key] =
          data.substr(space + key.size() + 1, nl - space - key.size() - 1);
      pos = nl + 3;
    } else {
      this->keyValuePairs[key] =
          data.substr(space + key.size() + 1, nl - space - key.size() - 1);
      pos = nl + 1;
    }
  }
  int nl = data.find("\n", pos + 1);
  this->keyValuePairs["message"] = data.substr(pos + 1, nl - pos);
}

bool GitCommit::has_parent() {
  return this->keyValuePairs.find("parent") != this->keyValuePairs.end();
}

std::string GitCommit::get_parent() { return this->keyValuePairs["parent"]; }
