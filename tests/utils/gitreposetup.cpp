#include "gitreposetup.h"
#include "boost/algorithm/string/trim.hpp"
#include <filesystem>
#include <fstream>
/**
Uses RAII to manage the setup and teardown of a sample Git repo
*/
namespace fs = std::filesystem;

const std::string FIRST_COMMIT = "1723ac93b92db1fc2c28de8e5da814136937f8c6";
const std::string SECOND_COMMIT = "6c2c22e7b5b7b1682e3c14668499e84141aca0d4";
const std::string THIRD_COMMIT = "ba570884af934a79081499203ba81750a945e3c5";
const fs::path VALID_GIT_PATH = fs::temp_directory_path() / "gitrepo";
const fs::path OLD_CWD = fs::current_path();
const fs::path TEST_FILE_PATH = OLD_CWD / "gitrepo";

GitRepoSetup::GitRepoSetup() { setup(); }
GitRepoSetup::~GitRepoSetup() { teardown(); }

void GitRepoSetup::setup() {
  // set up a git repo with an actual commit
  fs::copy(TEST_FILE_PATH, VALID_GIT_PATH, fs::copy_options::recursive);

  fs::current_path(VALID_GIT_PATH);
}

void GitRepoSetup::teardown() {
  fs::remove_all(VALID_GIT_PATH);
  fs::current_path(OLD_CWD);
}

std::string GitRepoSetup::get_file_contents(const fs::path &path) {
  std::ifstream file(path);
  if (file.is_open()) {
    std::string contents((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    boost::trim_right(contents);
    file.close();
    return contents;
  } else {
    throw std::runtime_error("Could not open file: " + path.string());
  }
}
