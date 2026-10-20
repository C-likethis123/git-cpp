#include "parsers/LsTreeParser.h"
#include "TclapParseScope.h"

LsTreeOptions LsTreeParser::parse(const std::vector<std::string> &args) {
  TclapParseScope scope;
  auto input = args;
  TCLAP::CmdLine cmd("ls-tree", ' ', "0.1");
  TCLAP::UnlabeledValueArg<std::string> treeObjArg(
      "tree-ish", "commit ID of tree object", true, "HEAD", "string");
  TCLAP::UnlabeledMultiArg<std::string> filePathPatternsArg(
      "path", "patterns that matches file names", false, "", "string");
  cmd.ignoreUnmatched(true);
  cmd.add(treeObjArg);
  cmd.add(filePathPatternsArg);
  cmd.parse(input);
  return {treeObjArg.getValue(), filePathPatternsArg.getValue()};
}
