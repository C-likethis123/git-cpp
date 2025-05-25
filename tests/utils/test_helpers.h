#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP

#include "catch2/catch.hpp"
#include <iostream>
#include <sstream>

#define REQUIRE_STDOUT_VALUE(text, value)                                      \
  do {                                                                         \
    std::stringstream buffer;                                                  \
    std::streambuf *oldCout = std::cout.rdbuf();                               \
    std::cout.rdbuf(buffer.rdbuf());                                           \
    text;                                                                      \
    std::cout.rdbuf(oldCout);                                                  \
    REQUIRE(buffer.str() == value);                                            \
  } while (false)

#endif // TEST_HELPERS_HP
