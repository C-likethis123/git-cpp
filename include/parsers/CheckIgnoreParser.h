#ifndef CHECKIGNOREPARSER_H
#define CHECKIGNOREPARSER_H

#include <string>
#include <vector>

struct CheckIgnoreOptions {
  std::vector<std::string> filePathPatterns;
};

class CheckIgnoreParser {
public:
  static CheckIgnoreOptions parse(const std::vector<std::string> &args);
};

#endif // CHECKIGNOREPARSER_H
