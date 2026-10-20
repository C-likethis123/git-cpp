#ifndef CHECKOUTPARSER_H
#define CHECKOUTPARSER_H

#include <string>
#include <vector>

struct CheckoutOptions {
  bool createNewBranch;
  std::string commit;
  std::string startPoint;
  bool startPointSet;
};

class CheckoutParser {
public:
  static CheckoutOptions parse(const std::vector<std::string> &args);
};

#endif // CHECKOUTPARSER_H
