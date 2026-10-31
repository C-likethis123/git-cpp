#include "catch2/catch.hpp"
#include "index.h"
#include "util.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"
#include "utils/test_helpers.h"

TEST_CASE("index preserves all conflict stages and resolves them on add", "[index]") {
  GitRepoSetup setup;
  const auto repo = GitRepository::find();
  GitIndex::Entries entries;
  for (auto stage : {IndexStage::Theirs, IndexStage::Base, IndexStage::Ours}) {
    entries.emplace(GitIndex::EntryKey{"conflict", stage},
                    GitIndexEntry(0, 0, 0, 0, 0, 0, 0100644, 0, 0, 0, 0, 0, 0,
                                  HEAD_COMMIT, false, false, stage, 8, "conflict"));
  }
  GitIndex(2, entries).save();
  const auto original = read_file(repo.repo_path("index"));
  REQUIRE(read_uint32_from_bytes(original, 8) == 3);
  // Each eight-character pathname occupies a 72-byte index entry.
  for (uint32_t i = 0; i < 3; ++i) {
    const auto flags = read_uint32_from_bytes(original, 12 + i * 72 + 60) >> 16;
    CHECK(((flags >> 12) & 3) == i + 1);
  }
  auto index = GitIndex::read();
  REQUIRE_THROWS_WITH(index.write_tree(), "Cannot write tree: unmerged path conflict");
  index.save();
  CHECK(read_file(repo.repo_path("index")) == original);
  std::stringstream output;
  {
    test_helpers::StdoutGuard guard(output.rdbuf());
    index.scan_status();
  }
  CHECK_THAT(output.str(), Catch::Matchers::Contains(
                               "Unmerged paths:\n\tunmerged: conflict\n"));

  file_utils::create_file("conflict", "resolved content");
  index.add_file("conflict");
  index.save();
  const auto resolved = read_file(repo.repo_path("index"));
  REQUIRE(read_uint32_from_bytes(resolved, 8) == 1);
  CHECK(((read_uint32_from_bytes(resolved, 72) >> 28) & 3) == 0);
  CHECK_NOTHROW(GitIndex::read().write_tree());
}
