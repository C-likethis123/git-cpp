#include "blob.h"

#include <string>

#include "repository.h"

GitBlob::GitBlob(const std::string &blobData)
    : blobData(blobData), GitObject("blob"){};

std::string GitBlob::serialise(GitRepository &repo) { return blobData; }

void GitBlob::deserialise(const std::string &data) { this->blobData = data; }
