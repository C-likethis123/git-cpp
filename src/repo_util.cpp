#include "pack_util.h"
#include "repository.h"
#include "util.h"
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Recursively resolves loose refs, falling back to packed refs.
std::string resolve_ref(const fs::path &ref_path, GitRepository &repo) {
  if (fs::exists(ref_path)) {
    const std::string ref = read_file(ref_path, true);
    if (ref.compare(0, 5, "ref: ") == 0) {
      return resolve_ref(repo.repo_path(ref.substr(5)), repo);
    }
    if (ref.empty()) {
      throw std::runtime_error("Unable to read reference: " +
                               ref_path.string());
    }
    return ref;
  }

  const fs::path packed_path = repo.repo_path("packed-refs");
  if (fs::exists(packed_path)) {
    std::ifstream file(packed_path);
    if (!file.is_open()) {
      throw std::runtime_error("Unable to read packed refs: " +
                               packed_path.string());
    }
    const std::string ref_name =
        ref_path.lexically_relative(repo.repo_path("")).generic_string();
    std::string line;
    while (std::getline(file, line)) {
      if (line.empty() || line[0] == '#' || line[0] == '^') {
        continue;
      }
      const auto space = line.find(' ');
      if (space != 40 ||
          line.substr(0, space).find_first_not_of("0123456789abcdefABCDEF") !=
              std::string::npos ||
          space + 1 == line.size() ||
          line.find_first_of(" \t\r", space + 1) != std::string::npos) {
        throw std::runtime_error("Malformed packed reference: " + line);
      }
      if (line.substr(space + 1) == ref_name) {
        return line.substr(0, space);
      }
    }
    if (file.bad()) {
      throw std::runtime_error("Unable to read packed refs: " +
                               packed_path.string());
    }
  }
  throw std::runtime_error("Reference not found: " + ref_path.string());
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
