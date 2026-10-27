#ifndef CATFILEPARSER_H
#define CATFILEPARSER_H

#include "object.h"

#include <string>
#include <vector>

struct CatfileOptions {
  GitObjectType type;
  std::string hash;
};

class CatfileParser {
public:
  static CatfileOptions parse(const std::vector<std::string> &args);
};

#endif // CATFILEPARSER_H
