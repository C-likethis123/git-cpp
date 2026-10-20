#ifndef LSTREEPARSER_H
#define LSTREEPARSER_H

#include <string>
#include <vector>

struct LsTreeOptions {
  std::string treeObject;
  std::vector<std::string> filePathPatterns;
};

class LsTreeParser {
public:
  static LsTreeOptions parse(const std::vector<std::string> &args);
};

#endif // LSTREEPARSER_H
