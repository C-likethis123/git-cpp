#ifndef LOGPARSER_H
#define LOGPARSER_H

#include <string>
#include <vector>

struct LogOptions {
  std::string commit;
};

class LogParser {
public:
  static LogOptions parse(const std::vector<std::string> &args);
};

#endif // LOGPARSER_H
