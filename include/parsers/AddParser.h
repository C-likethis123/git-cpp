#ifndef ADDPARSER_H
#define ADDPARSER_H

#include <string>
#include <vector>

struct AddOptions {
  std::vector<std::string> paths;
};

class AddParser {
public:
  static AddOptions parse(const std::vector<std::string> &args);
};

#endif // ADDPARSER_H
