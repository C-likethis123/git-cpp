#ifndef STATUS_FORMATTER_H
#define STATUS_FORMATTER_H

#include <iosfwd>
#include <string>
#include <vector>

namespace status_formatter {

struct Changes {
  std::vector<std::string> modified;
  std::vector<std::string> added;
  std::vector<std::string> deleted;
};

struct StatusResult {
  Changes staged;
  Changes unstaged;
  std::vector<std::string> untracked;
};

// Detect whether standard output supports automatic colour output.
bool use_status_colour();
void print_status(const StatusResult &status, std::ostream &out,
                  bool colour_enabled);

} // namespace status_formatter

#endif // STATUS_FORMATTER_H
