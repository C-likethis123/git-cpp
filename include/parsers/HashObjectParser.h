#ifndef HASHOBJECTPARSER_H
#define HASHOBJECTPARSER_H

#include <string>
#include <vector>

struct HashObjectOptions {
  bool write;
  std::string type;
  std::string path;
};

class HashObjectParser {
public:
  static HashObjectOptions parse(const std::vector<std::string> &args);
};

#endif // HASHOBJECTPARSER_H
