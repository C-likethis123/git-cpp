#include "parsers/TagParser.h"
#include "TclapParseScope.h"

TagOptions TagParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("tag", ' ', "0.1");
  TCLAP::UnlabeledValueArg<std::string> tagArg("tagname", "Tag name", true,
                                               "HEAD", "tag name");
  TCLAP::UnlabeledValueArg<std::string> commitArg(
      "commit", "Commit to tag to. If not specified, defaults to HEAD", false,
      "HEAD", "commit to tag");
  cmd.ignoreUnmatched(true);
  cmd.add(tagArg);
  cmd.add(commitArg);
  cmd.parse(input);
  return {tagArg.getValue(), commitArg.getValue(), commitArg.isSet()};
}
