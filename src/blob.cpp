#include "blob.h"

GitBlob::GitBlob(const std::string& blobData) : blobData(blobData) {}

std::string GitBlob::serialise(GitRepository& repo){
    return blobData;
}

void GitBlob::deserialise(std::string& data) {
    blobData = data;
}