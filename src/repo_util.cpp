#include "repository.h"
#include "util.h"
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

// Recursively resolves ref
std::string resolve_ref(const fs::path &ref_path, GitRepository &repo) {
  std::string ref = read_file(ref_path, true);
  if (ref.substr(0, 4) == "ref:") {
    fs::path new_ref_path = repo.repo_path(ref.substr(5));
    return resolve_ref(new_ref_path, repo);
  }
  return ref;
}

std::pair<std::string, std::string>
read_git_object_data(GitRepository &repo, const std::string &sha) {
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

  if (fmt != "blob" && fmt != "commit" && fmt != "tree") {
    throw std::runtime_error("Unknown type: " + fmt);
  }

  // Read and validate object size
  auto y = raw.find('\0', x);
  std::string size_str = raw.substr(x, y - x);
  int size = std::stoi(size_str);
  if (size != raw.size() - y - 1) {
    throw std::runtime_error("Malformed object: bad length");
  }
  return std::pair<std::string, std::string>{fmt, raw.substr(y + 1)};
}
