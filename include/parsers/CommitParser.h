#ifndef COMMITPARSER_H
#define COMMITPARSER_H

#include <string>
#include <vector>

struct CommitOptions {
  std::string message;
};

class CommitParser {
public:
  static CommitOptions parse(const std::vector<std::string> &args);
};

#endif // COMMITPARSER_H
