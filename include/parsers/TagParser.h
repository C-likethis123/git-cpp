#ifndef TAGPARSER_H
#define TAGPARSER_H

#include <string>
#include <vector>

struct TagOptions {
  std::string tag;
  std::string commit;
  bool commitSet;
};

class TagParser {
public:
  static TagOptions parse(const std::vector<std::string> &args);
};

#endif // TAGPARSER_H
