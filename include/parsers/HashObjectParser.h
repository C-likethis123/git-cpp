#ifndef HASHOBJECTPARSER_H
#define HASHOBJECTPARSER_H

#include "object.h"

#include <string>
#include <vector>

struct HashObjectOptions {
  bool write;
  GitObjectType type;
  std::string path;
};

class HashObjectParser {
public:
  static HashObjectOptions parse(const std::vector<std::string> &args);
};

#endif // HASHOBJECTPARSER_H
