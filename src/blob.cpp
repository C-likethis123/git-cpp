#include "blob.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <string>

#include "repository.h"

GitBlob::GitBlob(const std::string &blobData)
    : blobData(blobData), GitObject(){};

std::string GitBlob::serialise(GitRepository &repo) { return blobData; }

void GitBlob::deserialise(const std::string &data) { this->blobData = data; }
