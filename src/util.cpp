#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/uuid/detail/sha1.hpp>

namespace fs = boost::filesystem;

bool create_file(const fs::path &filePath, const std::string& content = "") {
    try {
        // Create the file
        std::ofstream fileStream(filePath.string());

        // Check if the file is successfully opened
        if (fileStream.is_open()) {
            // Write content to the file if provided
            if (!content.empty()) {
                fileStream << content;
            }

            std::cout << "File created successfully: " << filePath << std::endl;
            return true;
        } else {
            std::cerr << "Error opening the file for writing: " << filePath << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}

std::string sha1_hexdigest(const std::string& data) {
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(data.data(), data.size());

    unsigned int digest[5];
    sha1.get_digest(digest);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned int i : digest) {
        ss << std::setw(8) << i;
    }
    return ss.str();
}
