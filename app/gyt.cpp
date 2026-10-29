#include "commands/add.h"
#include "commands/cat-file.h"
#include "commands/check-ignore.h"
#include "commands/checkout.h"
#include "commands/commit.h"
#include "commands/hash-object.h"
#include "commands/init.h"
#include "commands/log.h"
#include "commands/ls-files.h"
#include "commands/ls-tree.h"
#include "commands/merge.h"
#include "commands/show-ref.h"
#include "commands/status.h"
#include "commands/tag.h"
#include "tclap/CmdLine.h"
#include "tclap/ValuesConstraint.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  using CommandHandler = void (*)(std::vector<std::string> &);
  const std::map<std::string, CommandHandler> handlers{
      {"init", commands::init},
      {"add", commands::add},
      {"commit", commands::commit},
      {"status", commands::status},
      {"log", commands::log},
      {"checkout", commands::checkout},
      {"tag", commands::tag},
      {"cat-file", commands::catfile},
      {"hash-object", commands::hashobject},
      {"ls-files", commands::lsfiles},
      {"ls-tree", commands::lstree},
      {"show-ref", commands::showref},
      {"check-ignore", commands::checkignore},
      {"merge", commands::merge}};
  std::string command;
  {
    std::vector<std::string> commandNames;
    for (const auto &handler : handlers) {
      commandNames.push_back(handler.first);
    }
    TCLAP::ValuesConstraint<std::string> allowedCommands(commandNames);
    TCLAP::CmdLine cli("A Git implementation in C++. "
                       "Run 'gyt <command> --help' for command options.",
                       ' ', "0.1");
    TCLAP::UnlabeledValueArg<std::string> commandArg(
        "command", "Command to execute", true, "", &allowedCommands);
    cli.add(commandArg);
    // Leave command-specific options to the command's parser.
    cli.parse(argc > 1 ? 2 : 1, argv);
    command = commandArg.getValue();
  }
  try {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
      args.push_back(argv[i]);
    }
    handlers.at(command)(args);
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
