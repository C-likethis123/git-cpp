#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include "inih/INIReader.h"

namespace fs = boost::filesystem;
class GitRepository {
    public:
        GitRepository(std::string worktree, std::string gitdir, INIReader conf);
        GitRepository(std::string path, bool force);

        /**
        Creates all files needed to represent a git directory
        */
        std::string create(bool mkdir);

    protected:
        fs::path worktree;
        fs::path gitdir;
        INIReader conf;  // Note: Directly declare INIReader instead of using a reference
        INIReader loadConfigFile(fs::path path);
};

#endif //REPOSITORY_H
