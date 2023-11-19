#include "repository.h"
#include "util.h"
#include "inih/INIReader.h"

namespace fs = boost::filesystem;

GitRepository::GitRepository(std::string path, bool force)
    : worktree(path), gitdir(path + ".git/"), conf(loadConfigFile(path + ".git/")) {
    if (!force && !fs::is_directory(gitdir)) {
        std::cerr << "Not a git repository: " << gitdir << "\n";
        // Handle error condition, throw an exception or set some flag, etc.
    }

    int vers = conf.GetInteger("core", "repositoryformatversion", -1);
    if (vers != 0) {
        std::cerr << "Unsupported repositoryformatversion " << vers << "\n";
        // Handle error condition, throw an exception or set some flag, etc.
    }
}

/**
Creates all files needed to represent a git directory
*/
std::string GitRepository::create(bool mkdir) {
    if (!fs::exists(worktree)) {
        fs::create_directories(worktree);
    }
    fs::create_directories(worktree + "branches/");
    fs::create_directories(worktree + "objects/");
    fs::create_directories(worktree + "refs/heads/");
    fs::create_directories(worktree + "refs/tags/");

    create_file(worktree + "description", "Unnamed repository; edit this file 'description' to name the repository.\n");
    create_file(worktree + "HEAD", "ref: refs/heads/master\n");
    create_file(worktree + "config", "[core]"
                                     "\trepositoryformatversion = 0"
                                     "\tfilemode = false"
                                     "\tbare = false"
    );
    return worktree;
}

// Helper function to load the config file
INIReader GitRepository::loadConfigFile(std::string path) {
    INIReader config(path + "config");
    if (config.ParseError() < 0) {
        std::cerr << "Cannot load .git/config\n";
        // Handle error condition, throw an exception or set some flag, etc.
    }
    return config;
}
