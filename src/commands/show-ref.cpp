#include "commands/show-ref.h"
#include <iostream>

#include "repository.h"
#include "tclap/CmdLine.h"

namespace commands {
void showref(std::vector<std::string> &args) {
  TCLAP::CmdLine cmd("show-ref", ' ', "0.1");

  // defines arguments
  TCLAP::UnlabeledValueArg<std::string> patternArg(
      "pattern", "pattern to match", false, "", "pattern to match");

  cmd.add(patternArg);
  cmd.parse(args);

  // process args
  try {
    std::optional<GitRepository> repo = GitRepository::find();
    if (repo) {
      if (patternArg.isSet()) {
        std::string pattern = patternArg.getValue();
        std::cout << "Pattern: " << pattern << "\n";
      } else {
        std::cout << "No pattern provided.\n";
      }
    }
  } catch (std::runtime_error &err) {
    std::cerr << err.what() << "\n";
  }
}
} // namespace commands
