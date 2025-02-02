#include "blob.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <iostream>
#include <sstream>
#include <string>

#include "repository.h"
#include "util.h"

GitBlob::GitBlob(const std::string &blobData)
    : blobData(blobData), GitObject(){};

std::string GitBlob::serialise(GitRepository &repo) { return blobData; }

void GitBlob::deserialise(std::string &data) { this->blobData = data; }

/**
Writes a hash to the object/ directory
Computes the hash
Insert the header
zlib-compress everything
Writes the result in place
**/
std::string GitBlob::write(GitRepository &repo, bool write) {
  std::string data = serialise(repo);
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
}