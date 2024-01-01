#include <boost/format.hpp>

#include "object.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/detail/optional.hpp>

GitObject::GitObject() {
    this->init();
}

GitObject::GitObject(std::string& data) {
    this->deserialise(data);
}

void GitObject::deserialise(std::string& data) {

}

void GitObject::serialise(GitRepository& repo) {

}

void GitObject::init() {

}

void GitObject::read(GitRepository& repo, std::string& sha) {
    std::string dir = sha.substr(0, 2);
    std::string path = sha.substr(2);
    std::string file_path = (boost::format("objects/%/%") % dir % path).str();
    fs::path file_to_create = fs::path(file_path);
    fs::path paths = repo.file(file_to_create);

    if (!fs::is_regular_file(paths)){
        return;
    }
    std::ifstream input_file(paths.string(), std::ios_base::in | std::ios_base::binary);
    std::vector<char> decompressed_data;

    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_decompressor());
    in.push(input_file);
    try {
        boost::iostreams::copy(in, std::back_inserter(decompressed_data));
    } catch (const boost::iostreams::zlib_error& e) {
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
    std::cout << "Object type: " << fmt << std::endl;
    std::cout << "Object size: " << size << std::endl;
}

std::string write(GitRepository& repo){
    return "";
}