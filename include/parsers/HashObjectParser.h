#ifndef HASHOBJECTPARSER_H
#define HASHOBJECTPARSER_H

#include "tclap/CmdLine.h"
#include "tclap/SwitchArg.h"
#include <string>
#include <vector>

class HashObjectParser {
public:
  static HashObjectParser &get();
  void parse(std::vector<std::string> &args);
  const std::string &getPath() const;
  bool isWrite() const;
  const std::string &getType() const;

private:
  // hides constructors to avoid accidental instantiation
  HashObjectParser();
  HashObjectParser(const HashObjectParser &) = delete;
  HashObjectParser &operator=(const HashObjectParser &) = delete;
  HashObjectParser(HashObjectParser &&) = delete;
  HashObjectParser &operator=(HashObjectParser &&) = delete;
  ~HashObjectParser() = default;
  // data members
  TCLAP::CmdLine cmd;
  TCLAP::SwitchArg writeArg;
  TCLAP::ValueArg<std::string> typeArg;
  TCLAP::UnlabeledValueArg<std::string> pathArg;
};

#endif // HASHOBJECTPARSER_H
