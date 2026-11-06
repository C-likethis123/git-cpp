#include "catch2/catch.hpp"
#include "util.h"
#include <stdexcept>

TEST_CASE("SHA-1 preserves standard vectors and binary input", "[util][sha1]") {
  CHECK(sha1_hexdigest("") == "da39a3ee5e6b4b0d3255bfef95601890afd80709");
  CHECK(sha1_hexdigest("abc") == "a9993e364706816aba3e25717850c26c9cd0d89d");
  CHECK(sha1_hexdigest(std::string(1000000, 'a')) ==
        "34aa973cd4c4daa4f61eeb2bdbad27316534016f");
  CHECK(sha1_hexdigest(std::string("\x00\x01\x7f\x80\xff", 5)) ==
        "47f6e64339628a989dbed958beba5944d0ea6d7b");
}

TEST_CASE("SHA-1 handles padding and block boundaries", "[util][sha1]") {
  CHECK(sha1_hexdigest(std::string(55, 'a')) ==
        "c1c8bbdc22796e28c0e15163d20899b65621d65a");
  CHECK(sha1_hexdigest(std::string(56, 'a')) ==
        "c2db330f6083854c99d4b5bfb6e8f29f201be699");
  CHECK(sha1_hexdigest(std::string(63, 'a')) ==
        "03f09f5b158a7a8cdad920bddc29b81c18a551f5");
  CHECK(sha1_hexdigest(std::string(64, 'a')) ==
        "0098ba824b5c16427bd7a1122a5a442a25ec644d");
  CHECK(sha1_hexdigest(std::string(65, 'a')) ==
        "11655326c708d70319be2610e8a57d9a5b959d3b");
}

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
