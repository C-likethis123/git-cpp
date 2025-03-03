#include "blob.h"
#include "object.h"
#include "util.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/detail/optional.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <vector>

GitObject::GitObject() { this->init(); }

void GitObject::init() {}

GitObject *GitObject::read(GitRepository &repo, const std::string &sha) {
  std::string dir = sha.substr(0, 2);
  std::string path = sha.substr(2);
  std::string file_path = (boost::format("objects/%1%/%2%") % dir % path).str();
  fs::path file_to_create = fs::path(file_path);
  fs::path paths = repo.file(file_to_create);

  if (!fs::is_regular_file(paths)) {
    throw std::runtime_error(paths.string() + " is not found");
  }
  std::ifstream input_file(paths.string(),
                           std::ios_base::in | std::ios_base::binary);
  std::vector<char> decompressed_data;

  boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
  in.push(boost::iostreams::zlib_decompressor());
  in.push(input_file);
  try {
    boost::iostreams::copy(in, std::back_inserter(decompressed_data));
  } catch (const boost::iostreams::zlib_error &e) {
    std::cerr << "Zlib decompression error: " << e.what() << std::endl;
  }

  std::string raw(decompressed_data.begin(), decompressed_data.end());
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
    std::cout << "Object type: " << fmt << std::endl;
    std::cout << "Object size: " << size << std::endl;
    return new GitBlob(raw.substr(y + 1));
  } else if (fmt == "commit") {
    return new GitCommit(raw.substr(y + 1));
  } else {
    throw std::runtime_error("Unknown type");
  }
}

std::string GitObject::write(GitRepository &repo, std::string &type,
                             std::string &data, bool write) {
  if (type == "blob") {
    std::ostringstream oss;
    oss << "blob " << data.size() << '\0' << data;
    std::string result = oss.str();
    std::string sha = sha1_hexdigest(result);

    if (write) {
      std::string file_path =
          (boost::format("objects/%1%/%2%") % sha.substr(0, 2) % sha.substr(2))
              .str();
      fs::path file_to_create = fs::path(file_path);
      fs::path paths = repo.file(file_to_create);

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

std::string GitObject::find(GitRepository &repo, std::string &name,
                            std::string &fmt, bool follow) {
  return name;
}
