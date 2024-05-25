#include "blob.h"

#include <iostream>
#include <sstream>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "repository.h"
#include "util.h"

GitBlob::GitBlob(const std::string& blobData) : blobData(blobData), GitObject() {};

std::string GitBlob::serialise(GitRepository& repo){
    return blobData;
}

void GitBlob::deserialise(std::string& data) {
    this->blobData = data;
}

/**
Writes a hash to the object/ directory
Computes the hash
Insert the header
zlib-compress everything
Writes the result in place
**/
std::string GitBlob::write(GitRepository& repo) {
    std::string data = serialise(repo);
    std::string header = "blob " + std::to_string(data.size()) + "\x00";
    std::string result = header + data;
    std::string sha = sha1_hexdigest(result);

    std::string file_path = (boost::format("objects/%1%/%2%") % sha.substr(0,2) % sha.substr(2)).str();
    fs::path file_to_create = fs::path(file_path);
    fs::path paths = repo.file(file_to_create);

    // compress files
    std::stringstream compressed;
    boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(compressed);
    boost::iostreams::copy(boost::iostreams::basic_array_source<char>(result.data(), result.size()), out);
    create_file(paths, compressed.str());
    return sha;
}