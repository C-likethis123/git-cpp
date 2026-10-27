#include "catch2/catch.hpp"
#include "commands/hash-object.h"
#include "parsers/HashObjectParser.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"
#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("hashobject command", "[hashobject]") {
  GitRepoSetup gitRepoSetup;
  SECTION("Valid git hashobject command - defaults to a blob",
          "hashobject commit") {
    std::vector<std::string> args({"hash-object", "test"});
    REQUIRE_STDOUT_VALUE(commands::hashobject(args),
                         "9daeafb9864cf43055ae93beb0afd6c7d144bfa4\n");
  }
  SECTION("Valid git hashobject command - hashobject commit",
          "hashobject commit") {
    std::vector<std::string> args({"hash-object", "-t", "blob", "test"});
    REQUIRE_STDOUT_VALUE(commands::hashobject(args),
                         "9daeafb9864cf43055ae93beb0afd6c7d144bfa4\n");
  }
  SECTION("Valid git hashobject command - writes object into memory",
          "hashobject blob") {
    // create a file
    file_utils::create_file("test3", "test contents 3\n");
    std::vector<std::string> args({"hash-object", "-w", "test3"});
    REQUIRE_STDOUT_VALUE(commands::hashobject(args),
                         "7eaae456b03342fe252b72e0a030a274703379d1\n");
    REQUIRE(
        fs::exists(".git/objects/7e/aae456b03342fe252b72e0a030a274703379d1"));
  }
}

TEST_CASE("hashobject parser returns a Git object type", "[hashobject][parser]") {
  REQUIRE(HashObjectParser::parse({"hash-object", "test"}).type ==
          GitObjectType::Blob);
  for (const auto type : {GitObjectType::Blob, GitObjectType::Tree,
                          GitObjectType::Commit}) {
    const auto options =
        HashObjectParser::parse({"hash-object", "-t", to_string(type), "test"});
    REQUIRE(options.type == type);
    REQUIRE(options.path == "test");
  }
  REQUIRE_THROWS(HashObjectParser::parse(
      {"hash-object", "-t", "invalid", "test"}));
}
