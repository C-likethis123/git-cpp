#include "parsers/CommitParser.h"
#include "TclapParseScope.h"

CommitOptions CommitParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("commit", ' ', "0.1");
  cmd.setExceptionHandling(false);
  TCLAP::ValueArg<std::string> messageArg(
      "m", "", "Commit message", true, "", "message");
  cmd.add(messageArg);
  cmd.parse(input);
  return {messageArg.getValue()};
}
