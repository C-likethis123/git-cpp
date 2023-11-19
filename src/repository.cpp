#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include "inih/INIReader.h"

class GitRepository {
public:
    GitRepository(std::string &path, bool force = false)
        : worktree(path), gitdir(path + "/.git"), conf(loadConfigFile(path + "/.git/")) {
        if (!force || !boost::filesystem::is_directory(gitdir)) {
            std::cerr << "Not a git repository: " << gitdir << "\n";
            // Handle error condition, throw an exception or set some flag, etc.
        }

        int vers = conf.GetInteger("core", "repositoryformatversion", -1);
        if (vers != 0) {
            std::cerr << "Unsupported repositoryformatversion " << vers << "\n";
            // Handle error condition, throw an exception or set some flag, etc.
        }
    }

protected:
    std::string worktree;
    std::string gitdir;
    INIReader conf;  // Note: Directly declare INIReader instead of using a reference

    // Helper function to load the config file
    INIReader loadConfigFile(std::string& path) {
        INIReader config(path + "/config");
        if (config.ParseError() < 0) {
            std::cerr << "Cannot load .git/config\n";
            // Handle error condition, throw an exception or set some flag, etc.
        }
        return config;
    }
};
