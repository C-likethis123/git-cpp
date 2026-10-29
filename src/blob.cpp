#include "blob.h"

#include "repo_util.h"
#include <string>

GitBlob::GitBlob(const std::string &blobData)
    : blobData(blobData), GitObject(GitObjectType::Blob) {};

GitBlob GitBlob::read(const std::string &hash) {
  const std::string &sha = GitObject::find(hash);
  auto [fmt, payload] = read_git_object_data(sha);
  if (fmt != "blob") {
    throw std::runtime_error("Invalid blob object: " + hash);
  }
  return GitBlob(payload);
}

std::string GitBlob::serialise() { return blobData; }

void GitBlob::deserialise(const std::string &data) { this->blobData = data; }
