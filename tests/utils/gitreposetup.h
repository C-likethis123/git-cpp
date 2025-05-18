#ifndef GITREPOSETUP_H
#define GITREPOSETUP_H

#include <filesystem>

namespace fs = std::filesystem;
/**
Uses RAII to manage the setup and teardown of a sample Git repo
*/
extern const std::string FIRST_COMMIT;
extern const std::string SECOND_COMMIT;
extern const std::string THIRD_COMMIT;
extern const fs::path VALID_GIT_PATH;
extern const fs::path OLD_CWD;
extern const fs::path TEST_FILE_PATH;

class GitRepoSetup {
public:
  GitRepoSetup();
  ~GitRepoSetup();

  void setup();
  void teardown();

  static std::string get_file_contents(const fs::path &path);
};

#endif // GITREPOSETUP_H
