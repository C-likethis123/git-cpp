#ifndef LSTREEPARSER_H
#define LSTREEPARSER_H

#include "tclap/CmdLine.h"
#include <string>
#include <vector>

class LsTreeParser {
public:
  static LsTreeParser &get();
  void parse(std::vector<std::string> &args);
  std::vector<std::string> getFilePathPatterns() const;
  std::string getTreeObject() const;

private:
  // hides constructors to avoid accidental instantiation
  LsTreeParser();
  LsTreeParser(const LsTreeParser &) = delete;
  LsTreeParser &operator=(const LsTreeParser &) = delete;
  LsTreeParser(LsTreeParser &&) = delete;
  LsTreeParser &operator=(LsTreeParser &&) = delete;
  ~LsTreeParser() = default;
  // data members
  TCLAP::CmdLine cmd;
  TCLAP::UnlabeledValueArg<std::string> treeObjArg;
  TCLAP::UnlabeledMultiArg<std::string> filePathPatternsArg;
};

#endif // LSTREEPARSER_H
