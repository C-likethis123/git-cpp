#ifndef STATUSPARSER_H
#define STATUSPARSER_H

#include <string>
#include <vector>

class StatusParser {
public:
  static void parse(const std::vector<std::string> &args);
};

#endif // STATUSPARSER_H
