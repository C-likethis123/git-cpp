#include "object.h"
#include "blob.h"
#include "boost/algorithm/string/case_conv.hpp"
#include "commit.h"
#include "repo_util.h"
#include "repository.h"
#include "tree.h"
#include "util.h"
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

std::string to_string(GitObjectType type) {
  switch (type) {
  case GitObjectType::Blob:
    return "blob";
  case GitObjectType::Tree:
    return "tree";
  case GitObjectType::Commit:
    return "commit";
  }
  throw std::runtime_error("Unknown type");
}

GitObjectType git_object_type_from_string(const std::string &type) {
  if (type == "blob")
    return GitObjectType::Blob;
  if (type == "tree")
    return GitObjectType::Tree;
  if (type == "commit")
    return GitObjectType::Commit;
  throw std::runtime_error("Unknown type: " + type);
}

GitObject::GitObject() { this->init(); }
GitObject::GitObject(GitObjectType format) { this->format = format; }

void GitObject::init() {}

GitObject *GitObject::read(const std::string &sha) {
  GitRepository repo = GitRepository::find();
  auto [fmt, payload] = repo.has_loose_object(sha)
                            ? read_git_object_data(sha)
                            : read_from_pack(repo.dir("objects/pack") / sha, 0);

  if (fmt == "blob") {
    return new GitBlob(payload);
  } else if (fmt == "commit") {
    return new GitCommit(payload, sha);
  } else {
    return new GitTree(payload);
  }
}

std::string GitObject::write(GitObjectType type, const std::string &data,
                             bool write) {
  GitRepository repo = GitRepository::find();
  std::ostringstream oss;
  oss << to_string(type) << " " << data.size() << '\0' << data;
  std::string result = oss.str();
  std::string sha = sha1_hexdigest(result);

  if (write) {
    if (repo.has_loose_object(sha)) {
      return sha;
    }
    fs::path file_path = repo.object_path(sha);
    fs::path paths = repo.file(file_path, write);

    // compress files
    std::stringstream compressed;
    boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(compressed);
    boost::iostreams::copy(boost::iostreams::basic_array_source<char>(
                               result.data(), result.size()),
                           out);
    if (!create_file(paths, compressed.str())) {
      throw std::runtime_error("Failed to write object: " + sha);
    }
  }
  return sha;
}

// XXX: food for thought: how does git handle the situation where
// the branch name is the same as a tag?
std::string GitObject::find(const std::string &name, bool follow) {
  GitRepository repo = GitRepository::find();
  fs::path ref_path;
  std::string ref_name = boost::algorithm::to_lower_copy(name);
  if (ref_name == "head") {
    ref_path = repo.repo_path("HEAD");
  } else if (repo.has_branch(ref_name)) {
    ref_path = repo.branch_path(ref_name);
  } else if (repo.has_tag(ref_name)) {
    ref_path = repo.tag_path(ref_name);
  } else if (fs::exists(repo.repo_path("refs/remotes/" + ref_name))) {
    ref_path = repo.repo_path("refs/remotes/" + ref_name);
  } else if (repo.has_loose_object(ref_name)) {
    return ref_name;
  } else if (repo.has_pack_object(ref_name)) {
    return ref_name;
  } else {
    throw std::runtime_error(name + ": not a valid reference");
  }
  return resolve_ref(ref_path);
}

GitObjectType GitObject::get_type() const { return this->format; }
