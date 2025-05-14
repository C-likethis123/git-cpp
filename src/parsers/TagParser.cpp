#include "parsers/TagParser.h"
#include <string>
#include <vector>

void TagParser::parse(std::vector<std::string> &args) {
  cmd.reset();
  cmd.parse(args);
}

std::string TagParser::getTag() { return tagArg.getValue(); }
bool TagParser::isCommitSet() const { return commitArg.isSet(); }
std::string TagParser::getCommit() { return commitArg.getValue(); }

TagParser &TagParser::get() {
  static TagParser instance;
  return instance;
}

TagParser::TagParser()
    : cmd("tag", ' ', "0.1"),
      tagArg("tagname", "Tag name", true, "HEAD", "tag name"),
      commitArg("commit",
                "Commit to tag to. If not specified, defaults to HEAD", false,
                "HEAD", "commit to tag") {
  cmd.ignoreUnmatched(true);
  cmd.add(tagArg);
  cmd.add(commitArg);
}
