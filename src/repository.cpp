#include "repository.h"
#include "util.h"
#include "inih/INIReader.h"

namespace fs = boost::filesystem;

GitRepository::GitRepository(std::string path, bool force)
    : worktree(fs::path(path)), gitdir(fs::path(path) / ".git"), conf(loadConfigFile(fs::path(path) / ".git")) {
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
    fs::create_directories(gitdir / "branches");
    fs::create_directories(gitdir / "objects");
    fs::create_directories(gitdir / "refs/heads");
    fs::create_directories(gitdir / "refs/tags");

    create_file(gitdir / "description", "Unnamed repository; edit this file 'description' to name the repository.\n");
    create_file(gitdir / "HEAD", "ref: refs/heads/master\n");
    create_file(gitdir / "config", "[core]\n"
                                     "\trepositoryformatversion = 0\n"
                                     "\tfilemode = false\n"
                                     "\tbare = false\n"
    );
    return worktree.string();
}

// Helper function to load the config file
INIReader GitRepository::loadConfigFile(fs::path path) {
    fs::path configPath = path / "config";
    INIReader config(configPath.string());
    if (config.ParseError() < 0) {
        std::cerr << "Cannot load .git/config\n";
        // Handle error condition, throw an exception or set some flag, etc.
    }
    return config;
}
