#include <boost/filesystem.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = boost::filesystem;

std::string read_file(const fs::path &filePath, bool remove_newline = false) {
  try {
    // Open the file
    std::ifstream fileStream(filePath.string());

    // Check if the file is successfully opened
    if (fileStream.is_open()) {
      // Read the content of the file
      std::stringstream buffer;
      buffer << fileStream.rdbuf();
      std::string contents = buffer.str();
      if (remove_newline) {
        // TODO: replace with trim_left in boost, see if times improve
        contents = contents.substr(0, contents.find_last_not_of("\n") + 1);
      }
      return contents;
    } else {
      std::cerr << "Error opening the file for reading: " << filePath
                << std::endl;
      return "";
    }
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return "";
  }
}

bool create_file(const fs::path &filePath, const std::string &content = "") {
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
      std::cerr << "Error opening the file for writing: " << filePath
                << std::endl;
      return false;
    }
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return false;
  }
}

std::string sha1_hexdigest(const std::string &data) {
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

std::string binaryToHex(const std::string &binaryData) {
  std::ostringstream hexStream;
  for (unsigned char byte : binaryData) {
    hexStream << std::setw(2) << std::setfill('0') << std::hex
              << static_cast<int>(byte);
  }
  return hexStream.str();
}

std::string hexToBinary(const std::string &hexString) {
  std::string binaryData;
  for (size_t i = 0; i < hexString.size(); i += 2) {
    // Take two hex characters at a time
    std::string byteStr = hexString.substr(i, 2);

    // Convert hex pair to a single byte (char)
    char byte = static_cast<char>(std::stoi(byteStr, nullptr, 16));
    binaryData.push_back(byte);
  }
  return binaryData;
}

// available values: git blob, or git tree
// reference:
// https://unix.stackexchange.com/questions/450480/file-permission-with-six-octal-digits-in-git-what-does-it-mean
std::string get_file_type(int mode) {
  int file_type = ((mode & 0xF000) >> 12);
  return file_type == 0x4 ? "tree" : "blob";
}

fs::perms get_unix_permissions(int mode) {
  mode = mode & 0x1FF;
  boost::filesystem::perms permissions = fs::perms::no_perms;

  // Owner permissions
  if (mode & 0400)
    permissions |= boost::filesystem::perms::owner_read;
  if (mode & 0200)
    permissions |= boost::filesystem::perms::owner_write;
  if (mode & 0100)
    permissions |= boost::filesystem::perms::owner_exe;

  // Group permissions
  if (mode & 0040)
    permissions |= boost::filesystem::perms::group_read;
  if (mode & 0020)
    permissions |= boost::filesystem::perms::group_write;
  if (mode & 0010)
    permissions |= boost::filesystem::perms::group_exe;

  // Others permissions
  if (mode & 0004)
    permissions |= boost::filesystem::perms::others_read;
  if (mode & 0002)
    permissions |= boost::filesystem::perms::others_write;
  if (mode & 0001)
    permissions |= boost::filesystem::perms::others_exe;

  return permissions;
}