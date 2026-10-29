#ifndef MERGEPARSER_H
#define MERGEPARSER_H

#include <string>
#include <vector>

struct MergeOptions {
  std::string commit;
};

class MergeParser {
public:
  static MergeOptions parse(const std::vector<std::string> &args);
};

#endif // MERGEPARSER_H
