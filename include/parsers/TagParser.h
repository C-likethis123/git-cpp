#ifndef TAGPARSER_H
#define TAGPARSER_H

#include "tclap/CmdLine.h"
#include <string>
#include <vector>

class TagParser {
public:
  static TagParser &get();
  void parse(std::vector<std::string> &args);
  std::string getTag();
  bool isCommitSet() const;
  std::string getCommit();

private:
  TagParser();
  TCLAP::CmdLine cmd;
  TCLAP::UnlabeledValueArg<std::string> tagArg;
  TCLAP::UnlabeledValueArg<std::string> commitArg;
};

#endif // TAGPARSER_H
