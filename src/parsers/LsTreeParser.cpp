#include "parsers/LsTreeParser.h"
#include <string>
#include <vector>

void LsTreeParser::parse(std::vector<std::string> &args) {
  cmd.reset();
  cmd.parse(args);
}

std::string LsTreeParser::getTreeObject() const { return treeObjArg.getValue(); }
std::vector<std::string> LsTreeParser::getFilePathPatterns() const { return filePathPatternsArg.getValue(); }

LsTreeParser::LsTreeParser()
    : cmd("ls-tree", ' ', "0.1"),
      treeObjArg("tree-ish", "commit ID of tree object", true, "HEAD", "string"),
      filePathPatternsArg("path", "patterns that matches file names", false, "", "string") {
  cmd.ignoreUnmatched(true);
  cmd.add(treeObjArg);
  cmd.add(filePathPatternsArg);
}

LsTreeParser &LsTreeParser::get() {
  static LsTreeParser instance;
  return instance;
}
