#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <iostream>
#include <boost/filesystem.hpp>

#include "repository.h"

namespace fs = boost::filesystem;
class GitObject {
    public:
        GitObject();
        GitObject(std::string& data);

        void deserialise(std::string& data);
        void serialise(GitRepository& repo);
        void read(GitRepository& repo, std::string& sha);
        std::string write(GitRepository& repo);
        void init();

    protected:
        fs::path worktree;
        fs::path gitdir;
};

#endif //OBJECT_H
