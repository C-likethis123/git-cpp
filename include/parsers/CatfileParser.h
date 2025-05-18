#ifndef CATFILEPARSER_H
#define CATFILEPARSER_H

#include "tclap/CmdLine.h"
#include <string>
#include <vector>

class CatfileParser {
public:
  static CatfileParser &get();
  void parse(std::vector<std::string> &args);
  std::string getType() const;
  std::string getHash() const;

private:
  // hides constructors to avoid accidental instantiation
  CatfileParser();
  CatfileParser(const CatfileParser &) = delete;
  CatfileParser &operator=(const CatfileParser &) = delete;
  CatfileParser(CatfileParser &&) = delete;
  CatfileParser &operator=(CatfileParser &&) = delete;
  ~CatfileParser() = default;
  // data members
  TCLAP::CmdLine cmd;
  TCLAP::UnlabeledValueArg<std::string> typeArg;
  TCLAP::UnlabeledValueArg<std::string> objArg;
};

#endif // CATFILEPARSER_H
