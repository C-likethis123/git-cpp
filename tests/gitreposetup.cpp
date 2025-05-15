#include <filesystem>
/**
Uses RAII to manage the setup and teardown of a sample Git repo
*/
namespace fs = std::filesystem;
class GitRepoSetup {
public:
  GitRepoSetup() { setup(); }
  ~GitRepoSetup() { teardown(); }

  void setup() {
    // set up a git repo with an actual commit
    fs::copy(TEST_FILE_PATH, VALID_GIT_PATH, fs::copy_options::recursive);

    fs::current_path(VALID_GIT_PATH);
  }

  void teardown() {
    fs::remove_all(VALID_GIT_PATH);
    fs::current_path(OLD_CWD);
  }
};
