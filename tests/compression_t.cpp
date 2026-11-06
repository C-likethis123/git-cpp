#include "catch2/catch.hpp"
#include "object.h"
#include "repo_util.h"
#include "repository.h"
#include "util.h"
#include "utils/gitreposetup.h"
#include <zlib.h>

namespace {
std::string compress_with_zlib(const std::string &input) {
  uLongf size = compressBound(input.size());
  std::string output(size, '\0');
  REQUIRE(compress2(reinterpret_cast<Bytef *>(output.data()), &size,
                    reinterpret_cast<const Bytef *>(input.data()), input.size(),
                    Z_DEFAULT_COMPRESSION) == Z_OK);
  output.resize(size);
  return output;
}

std::string packed_blob(const std::string &payload) {
  size_t size = payload.size();
  unsigned char byte = 0x30 | (size & 0x0f);
  size >>= 4;
  std::string header;
  while (size != 0) {
    header.push_back(static_cast<char>(byte | 0x80));
    byte = size & 0x7f;
    size >>= 7;
  }
  header.push_back(static_cast<char>(byte));
  return header + compress_with_zlib(payload);
}
} // namespace

TEST_CASE("Written loose objects are complete zlib streams",
          "[compression][object]") {
  GitRepoSetup setup;
  GitRepository repo(".");
  std::string binary(100000, '\0');
  uint32_t state = 1;
  for (char &byte : binary) {
    state = state * 1664525u + 1013904223u;
    byte = static_cast<char>(state >> 24);
  }
  for (const auto &payload : {std::string(), std::string("a\0b\xff", 4),
                              std::string(100000, 'x'), binary}) {
    const auto sha = GitObject::write(GitObjectType::Blob, payload, true);
    const std::string raw =
        "blob " + std::to_string(payload.size()) + '\0' + payload;
    const auto compressed = read_file(repo.object_path(sha));
    std::string decoded(raw.size(), '\0');
    uLongf size = decoded.size();
    REQUIRE(uncompress(reinterpret_cast<Bytef *>(decoded.data()), &size,
                       reinterpret_cast<const Bytef *>(compressed.data()),
                       compressed.size()) == Z_OK);
    REQUIRE(size == raw.size());
    REQUIRE(decoded == raw);
    REQUIRE(read_git_object_data(sha) ==
            std::make_pair(std::string("blob"), payload));
  }
}

TEST_CASE("Loose object decompression rejects invalid streams",
          "[compression][object]") {
  GitRepoSetup setup;
  GitRepository repo(".");
  const std::string raw("blob 5\0hello", 12);
  const auto sha = sha1_hexdigest(raw);
  auto object_path = repo.object_path(sha);
  const auto path = repo.file(object_path, true);
  auto compressed = compress_with_zlib(raw);

  SECTION("truncated checksum") { compressed.pop_back(); }
  SECTION("corrupt checksum") { compressed.back() ^= 1; }
  SECTION("uncompressed data") { compressed = raw; }
  SECTION("empty input") { compressed.clear(); }

  REQUIRE(create_file(path, compressed));
  REQUIRE_THROWS(read_git_object_data(sha));
}

TEST_CASE("Pack decompression stops at the end of each zlib stream",
          "[compression][pack]") {
  GitRepoSetup setup;
  const fs::path path = "objects.pack";
  const std::string first = packed_blob("a");
  const std::string second_payload(100000, 'x');
  const std::string second = packed_blob(second_payload);
  std::string pack("PACK\0\0\0\2\0\0\0\2", 12);
  pack += first + second;
  pack += hexToBinary(sha1_hexdigest(pack));
  REQUIRE(create_file(path, pack));
  REQUIRE(read_from_pack(path, 0) ==
          std::make_pair(std::string("blob"), std::string("a")));
  REQUIRE(read_from_pack(path, first.size()) ==
          std::make_pair(std::string("blob"), second_payload));

  SECTION("truncated stream") {
    REQUIRE(create_file(path, pack.substr(0, 12 + first.size() - 1)));
    REQUIRE_THROWS(read_from_pack(path, 0));
  }
  SECTION("wrong uncompressed size") {
    pack[12] = 0x32; // Header claims two bytes; the stream contains one.
    REQUIRE(create_file(path, pack));
    REQUIRE_THROWS(read_from_pack(path, 0));
  }
}
