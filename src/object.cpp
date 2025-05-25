#include "object.h"
#include "blob.h"
#include "boost/algorithm/string/case_conv.hpp"
#include "commit.h"
#include "tree.h"
#include "util.h"
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

GitObject::GitObject() { this->init(); }
GitObject::GitObject(const std::string &format) { this->format = format; }

void GitObject::init() {}

GitObject *GitObject::read(GitRepository &repo, const std::string &sha) {
  fs::path file_path = repo.object_path(sha);
  fs::path paths = repo.file(file_path);

  if (!fs::is_regular_file(paths)) {
    throw std::runtime_error(paths.string() + " is not found");
  }
  std::ifstream input_file(paths.string(),
                           std::ios_base::in | std::ios_base::binary);
  std::stringstream decompressed_data;

  boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
  in.push(boost::iostreams::zlib_decompressor());
  in.push(input_file);
  try {
    boost::iostreams::copy(in, decompressed_data);
  } catch (const boost::iostreams::zlib_error &e) {
    std::cerr << "Zlib decompression error: " << e.what() << std::endl;
  }

  std::string raw = decompressed_data.str();
  // Read object type
  auto x = raw.find(' ');
  std::string fmt = raw.substr(0, x);

  // Read and validate object size
  auto y = raw.find('\0', x);
  std::string size_str = raw.substr(x, y - x);
  int size = std::stoi(size_str);
  if (size != raw.size() - y - 1) {
    throw std::runtime_error("Malformed object: bad length");
  }

  if (fmt == "blob") {
    return new GitBlob(raw.substr(y + 1));
  } else if (fmt == "commit") {
    return new GitCommit(raw.substr(y + 1), sha);
  } else if (fmt == "tree") {
    return new GitTree(raw.substr(y + 1));
  } else {
    throw std::runtime_error("Unknown type");
  }
}

std::string GitObject::write(GitRepository &repo, const std::string &type,
                             const std::string &data, bool write) {
  if (type == "blob") {
    std::ostringstream oss;
    oss << "blob " << data.size() << '\0' << data;
    std::string result = oss.str();
    std::string sha = sha1_hexdigest(result);

    if (write) {
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
      create_file(paths, compressed.str());
    }
    return sha;
  } else {
    throw std::runtime_error("Unknown type");
  }
}

// XXX: food for thought: how does git handle the situation where
// the branch name is the same as a tag?
std::string GitObject::find(GitRepository &repo, const std::string &name,
                            bool follow) {
  fs::path ref_path;
  std::string ref_name = boost::algorithm::to_lower_copy(name);
  if (ref_name == "head") {
    ref_path = repo.repo_path("HEAD");
  } else if (repo.has_branch(ref_name)) {
    ref_path = repo.branch_path(ref_name);
  } else if (fs::exists(repo.repo_path("refs/tags/" + ref_name))) {
    ref_path = repo.repo_path("refs/tags/" + ref_name);
  } else if (fs::exists(repo.repo_path("refs/remotes/" + ref_name))) {
    ref_path = repo.repo_path("refs/remotes/" + ref_name);
  } else if (repo.has_object(ref_name)) {
    return ref_name;
  } else {
    throw std::runtime_error(name + ": not a valid reference");
  }
  return resolve_ref(ref_path, repo);
}

std::string GitObject::get_type() const { return this->format; }
