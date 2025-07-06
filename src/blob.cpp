#include "blob.h"

#include "repository.h"
#include "util.h"
#include <string>

GitBlob::GitBlob(const std::string &blobData)
    : blobData(blobData), GitObject("blob"){};

GitBlob GitBlob::read(GitRepository &repo, const std::string &hash) {
  const std::string &sha = GitObject::find(repo, hash);
  auto [fmt, payload] = read_git_object_data(repo, sha);
  if (fmt != "blob") {
    throw std::runtime_error("Invalid blob object: " + hash);
  }
  return GitBlob(payload);
}

std::string GitBlob::serialise(GitRepository &repo) { return blobData; }

void GitBlob::deserialise(const std::string &data) { this->blobData = data; }
