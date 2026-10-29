#include "commands/merge.h"
#include "parsers/MergeParser.h"
#include <stdexcept>

namespace commands {
void merge(std::vector<std::string> &args) {
  const auto options = MergeParser::parse(args);
  // TODO: Merge options.commit into HEAD.
  throw std::runtime_error("merge is not implemented yet: " + options.commit);
}
} // namespace commands
