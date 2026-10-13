#ifndef ADDPARSER_H
#define ADDPARSER_H

#include "tclap/CmdLine.h"
#include <string>
#include <vector>

class AddParser {
public:
  static AddParser &get();
  void parse(std::vector<std::string> &args);
  std::vector<std::string> getPaths() const;

private:
  // hides constructors to avoid accidental instantiation
  AddParser();
  AddParser(const AddParser &) = delete;
  AddParser &operator=(const AddParser &) = delete;
  AddParser(AddParser &&) = delete;
  AddParser &operator=(AddParser &&) = delete;
  ~AddParser() = default;
  // data members
  TCLAP::CmdLine cmd;
  TCLAP::UnlabeledMultiArg<std::string> pathArg;
};

#endif // ADDPARSER_H
