#include "pack_util.h"
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

std::pair<std::string, std::string> read_from_pack(const fs::path &pack_path,
                                                   size_t offset) {
  std::ifstream file(pack_path, std::ios::binary);

  // Skip pack file header (12 bytes: signature + version + object count)
  file.seekg(12 + offset);

  // Read the object header to get type and size
  auto [object_type, object_size] = read_pack_object_header(file);

  // For now, handle only non-delta objects (types 1-4)
  if (object_type >= 6) {
    throw std::runtime_error("Delta objects not yet implemented");
  }

  // Read and decompress the object data
  std::string compressed_data = read_bytes(file, object_size);

  // Decompress using zlib
  std::stringstream compressed_stream(compressed_data);
  std::stringstream decompressed_stream;

  boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
  in.push(boost::iostreams::zlib_decompressor());
  in.push(compressed_stream);

  try {
    boost::iostreams::copy(in, decompressed_stream);
  } catch (const boost::iostreams::zlib_error &e) {
    throw std::runtime_error("Zlib decompression error: " +
                             std::string(e.what()));
  }

  return {type_to_string(object_type), decompressed_stream.str()};
}
