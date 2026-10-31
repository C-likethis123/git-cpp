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

TEST_CASE("index from tree preserves nested paths modes and object IDs",
          "[index][from-tree]") {
  GitRepoSetup setup;
  const auto repo = GitRepository::find();
  const auto original_index = read_file(repo.repo_path("index"));
  const auto blob = GitObject::write(GitObjectType::Blob, "tree content", true);
  GitTree child;
  child.pathNames = {"run"};
  child.fileEntries.emplace("run", std::make_tuple(0100755, blob));
  const auto child_sha =
      GitObject::write(GitObjectType::Tree, child.serialise(), true);
  GitTree root;
  root.pathNames = {"nested", "plain", "link", "submodule"};
  root.fileEntries = {{"nested", {0040000, child_sha}},
                      {"plain", {0100644, blob}},
                      {"link", {0120000, blob}},
                      {"submodule", {0160000, HEAD_COMMIT}}};
  const auto root_sha =
      GitObject::write(GitObjectType::Tree, root.serialise(), true);
  file_utils::create_file("plain", "different worktree content");

  auto index = GitIndex::from_tree(root);
  CHECK(read_file(repo.repo_path("index")) == original_index);
  CHECK(read_file("plain") == "different worktree content");
  CHECK_FALSE(fs::exists("nested/run"));
  REQUIRE(index.write_tree() == root_sha);
  index.save();
  CHECK(read_uint32_from_bytes(read_file(repo.repo_path("index")), 8) == 4);
  CHECK(GitIndex::read().write_tree() == root_sha);
}

TEST_CASE("index from empty tree creates an empty index", "[index][from-tree]") {
  GitRepoSetup setup;
  auto index = GitIndex::from_tree(GitTree{});
  index.save();
  CHECK(read_uint32_from_bytes(
            read_file(GitRepository::find().repo_path("index")), 8) == 0);
  CHECK(GitIndex::read().write_tree() ==
        "4b825dc642cb6eb9a060e54bf8d69288fbee4904");
}

TEST_CASE("index from tree rejects invalid entries", "[index][from-tree]") {
  GitRepoSetup setup;
  for (const std::string name : {"", ".", "..", ".git", "a/b"}) {
    GitTree tree;
    tree.pathNames = {name};
    tree.fileEntries.emplace(name, std::make_tuple(0100644, HEAD_COMMIT));
    CHECK_THROWS_WITH(GitIndex::from_tree(tree),
                      "Invalid tree entry name: " + name);
  }
  GitTree tree;
  tree.pathNames = {"invalid"};
  tree.fileEntries.emplace("invalid", std::make_tuple(0, HEAD_COMMIT));
  CHECK_THROWS_WITH(GitIndex::from_tree(tree), "Unsupported tree mode: invalid");
}
