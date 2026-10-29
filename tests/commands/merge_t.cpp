#include "catch2/catch.hpp"
#include "commands/merge.h"
#include "parsers/MergeParser.h"

TEST_CASE("merge parser accepts one target", "[merge]") {
  REQUIRE(MergeParser::parse({"merge", "feature"}).commit == "feature");
  REQUIRE(MergeParser::parse({"merge", "abc123"}).commit == "abc123");
}

TEST_CASE("merge parser rejects missing or extra arguments", "[merge]") {
  REQUIRE_THROWS(MergeParser::parse({"merge"}));
  REQUIRE_THROWS(MergeParser::parse({"merge", "feature", "other"}));
  REQUIRE_THROWS(MergeParser::parse({"merge", "--unknown", "feature"}));
}

TEST_CASE("merge command reports the unimplemented scaffold", "[merge]") {
  std::vector<std::string> args = {"merge", "feature"};
  REQUIRE_THROWS_WITH(commands::merge(args),
                      "merge is not implemented yet: feature");
}
