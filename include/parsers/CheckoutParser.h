#ifndef CHECKOUTPARSER_H
#define CHECKOUTPARSER_H

#include "tclap/CmdLine.h"
#include <string>
#include <vector>

class CheckoutParser {
public:
  static CheckoutParser &get();
  void parse(std::vector<std::string> &args);
  bool isCommitSet() const;
  std::string getCommit();
  bool isCreateNewBranch() const;
  std::string getStartPoint();
  bool isStartPointSet() const;

private:
  // hides constructors to avoid accidental instantiation
  CheckoutParser();
  CheckoutParser(const CheckoutParser &) = delete;
  CheckoutParser &operator=(const CheckoutParser &) = delete;
  CheckoutParser(CheckoutParser &&) = delete;
  CheckoutParser &operator=(CheckoutParser &&) = delete;
  ~CheckoutParser() = default;
  // data members
  TCLAP::CmdLine cmd;
  TCLAP::SwitchArg branchArg;
  TCLAP::UnlabeledValueArg<std::string> commitArg;
  TCLAP::UnlabeledValueArg<std::string> startPointArg;
};

#endif // CHECKOUTPARSER_H
