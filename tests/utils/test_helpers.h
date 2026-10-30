#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP

#include "catch2/catch.hpp"
#include <iostream>
#include <sstream>

namespace test_helpers {
class StdoutGuard {
public:
  explicit StdoutGuard(std::streambuf *buffer) : previous(std::cout.rdbuf(buffer)) {}
  ~StdoutGuard() { std::cout.rdbuf(previous); }
  StdoutGuard(const StdoutGuard &) = delete;
  StdoutGuard &operator=(const StdoutGuard &) = delete;

private:
  std::streambuf *previous;
};
} // namespace test_helpers

#define REQUIRE_STDOUT_VALUE(text, value)                                       \
  do {                                                                        \
    std::stringstream buffer;                                                 \
    {                                                                         \
      test_helpers::StdoutGuard guard(buffer.rdbuf());                          \
      text;                                                                   \
    }                                                                         \
    REQUIRE(buffer.str() == value);                                            \
  } while (false)

#endif // TEST_HELPERS_HPP
