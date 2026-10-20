#ifndef INITPARSER_H
#define INITPARSER_H

#include <string>
#include <vector>

struct InitOptions {
  std::string path;
};

class InitParser {
public:
  static InitOptions parse(const std::vector<std::string> &args);
};

#endif // INITPARSER_H
