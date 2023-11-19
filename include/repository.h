#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
class GitRepository {
    public:
        GitRepository(fs::path worktree, fs::path gitdir);
        GitRepository(std::string path, bool force);

        /**
        Creates all files needed to represent a git directory
        */
        std::string create(bool mkdir);

    protected:
        fs::path worktree;
        fs::path gitdir;
};

#endif //REPOSITORY_H
