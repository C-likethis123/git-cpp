#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

bool create_file(const fs::path &filePath, const std::string& content = "") {
    try {
        // Create the file
        fs::ofstream fileStream(filePath);

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
