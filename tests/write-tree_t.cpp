#include "catch2/catch.hpp"
#include "index.h"
#include "object.h"
#include "repo_util.h"
#include "tree.h"
#include "util.h"
#include "utils/file_utils.h"
#include "utils/gitreposetup.h"

namespace {
GitIndexEntry tree_entry(const std::string &path, uint32_t mode,
                         const std::string &sha,
                         IndexStage stage = IndexStage::Resolved) {
  return GitIndexEntry(0, 0, 0, 0, 0, 0, mode, 0, 0, 0, 0, 0, 0, sha, false,
                       false, stage, path.size(), path);
}

std::string tree_record(const std::string &mode, const std::string &name,
                        const std::string &sha) {
  return mode + " " + name + '\0' + hexToBinary(sha);
}
} // namespace

TEST_CASE("write tree builds nested snapshots in Git order", "[write-tree]") {
  GitRepoSetup setup;
  GitRepository repo = GitRepository::find();
  const auto blob =
      GitObject::write(GitObjectType::Blob, "staged content\n", true);
  GitIndex index(
      2,
      {
          {{"foo/nested/run", IndexStage::Resolved},
           tree_entry("foo/nested/run", 0100755, blob)},
          {{"foo.txt", IndexStage::Resolved},
           tree_entry("foo.txt", 0100644, blob)},
          {{"foo0", IndexStage::Resolved}, tree_entry("foo0", 0100644, blob)},
          {{"link", IndexStage::Resolved}, tree_entry("link", 0120000, blob)},
          {{"submodule", IndexStage::Resolved},
           tree_entry("submodule", 0160000, HEAD_COMMIT)},
      });

  const auto root_sha = index.write_tree();
  const auto root = GitTree::read(root_sha);
  const auto foo_sha = std::get<1>(root.fileEntries.at("foo"));
  const auto foo = GitTree::read(foo_sha);
  const auto nested_sha = std::get<1>(foo.fileEntries.at("nested"));
  const auto nested = GitTree::read(nested_sha);
  REQUIRE(std::get<0>(nested.fileEntries.at("run")) == 0100755);
  REQUIRE(std::get<1>(nested.fileEntries.at("run")) == blob);
  REQUIRE(read_git_object_data(nested_sha).second ==
          tree_record("100755", "run", blob));
  REQUIRE(read_git_object_data(root_sha).second ==
          tree_record("100644", "foo.txt", blob) +
              tree_record("40000", "foo", foo_sha) +
              tree_record("100644", "foo0", blob) +
              tree_record("120000", "link", blob) +
              tree_record("160000", "submodule", HEAD_COMMIT));
  REQUIRE(index.write_tree() == root_sha);
}

TEST_CASE("write tree uses staged blobs without reading HEAD or worktree",
          "[write-tree]") {
  GitRepoSetup setup;
  GitRepository repo = GitRepository::find();
  GitIndex index(2, {});
  file_utils::create_file("staged", "original content");
  index.add_file("staged");
  index.save();
  file_utils::create_file("staged", "unstaged content");
  file_utils::create_file("untracked", "not staged");
  fs::remove(repo.repo_path("HEAD"));

  const auto tree = GitTree::read(GitIndex::read().write_tree());
  REQUIRE(tree.fileEntries.size() == 1);
  const auto blob = std::get<1>(tree.fileEntries.at("staged"));
  REQUIRE(read_git_object_data(blob).second == "original content");

  // A staged deletion is absence from the next index snapshot.
  GitIndex empty(2, {});
  empty.save();
  REQUIRE(GitIndex::read().write_tree() ==
          "4b825dc642cb6eb9a060e54bf8d69288fbee4904");
  fs::remove(repo.repo_path("index"));
  REQUIRE(GitIndex::read().write_tree() ==
          "4b825dc642cb6eb9a060e54bf8d69288fbee4904");
}

TEST_CASE("write tree rejects unresolved index entries", "[write-tree]") {
  GitRepoSetup setup;
  GitRepository repo = GitRepository::find();
  for (const auto stage :
       {IndexStage::Base, IndexStage::Ours, IndexStage::Theirs}) {
    GitIndex index(2, {{{"conflict", stage},
                        tree_entry("conflict", 0100644, HEAD_COMMIT, stage)}});
    index.save();
    REQUIRE_THROWS_WITH(GitIndex::read().write_tree(),
                        "Cannot write tree: unmerged path conflict");
  }
}

TEST_CASE("write tree rejects invalid paths and file directory collisions",
          "[write-tree]") {
  GitRepoSetup setup;
  GitRepository repo = GitRepository::find();
  for (const std::string path :
       {"/absolute", "../outside", "a//b", "a/", ".git/config"}) {
    GitIndex index(2, {{{path, IndexStage::Resolved},
                        tree_entry(path, 0100644, HEAD_COMMIT)}});
    REQUIRE_THROWS(index.write_tree());
  }
  GitIndex collision(2, {
                            {{"a", IndexStage::Resolved},
                             tree_entry("a", 0100644, HEAD_COMMIT)},
                            {{"a/b", IndexStage::Resolved},
                             tree_entry("a/b", 0100644, HEAD_COMMIT)},
                        });
  REQUIRE_THROWS(collision.write_tree());
}
