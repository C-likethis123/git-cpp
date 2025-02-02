#ifndef BLOB_H
#define BLOB_H

#include <string>
#include <boost/filesystem.hpp>

#include "object.h"

namespace fs = boost::filesystem;
class GitBlob : public GitObject {
    public:
        GitBlob(const std::string& data = std::string(""));

        void deserialise(std::string& data) override;
        std::string serialise(GitRepository& repo) override;
        std::string write(GitRepository& repo, bool write) override;

        void init();

    protected:
        std::string blobData;
};

#endif // BLOB_H
