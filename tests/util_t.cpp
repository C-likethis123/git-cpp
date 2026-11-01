#include "catch2/catch.hpp"
#include "util.h"
#include <stdexcept>

TEST_CASE("binaryToHex preserves every byte as two lowercase hex digits",
          "[util]") {
  CHECK(binaryToHex("").empty());
  CHECK(binaryToHex("Git") == "476974");
  CHECK(binaryToHex(std::string("\x00\x01\x0f\x10\x7f\x80\xff", 7)) ==
        "00010f107f80ff");
}

TEST_CASE("hexToBinary preserves the existing hexadecimal parsing behavior",
          "[util]") {
  CHECK(hexToBinary("").empty());
  CHECK(hexToBinary("476974") == "Git");
  CHECK(hexToBinary("00010F107f80FF") ==
        std::string("\x00\x01\x0f\x10\x7f\x80\xff", 7));
  CHECK(hexToBinary("abc") == std::string("\xab\x0c", 2));
  CHECK(hexToBinary("1g") == std::string("\x01", 1));
  CHECK_THROWS_AS(hexToBinary("zz"), std::invalid_argument);
}
