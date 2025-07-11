#include "repository.h"
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

uint32_t read_uint32_from_bytes(const std::string &s, size_t offset) {
  if (offset + 4 > s.size())
    throw std::runtime_error("out of range");
  uint32_t value;
  std::memcpy(&value, s.data() + offset, sizeof(value));
  return ntohl(value); // if data is big-endian (common in file formats)
}

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
        boost::trim_right(contents);
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

      // std::cout << "File created successfully: " << filePath << std::endl;
      return true;
    } else {
      std::cerr << "Error opening the file for writing: " << filePath << "\n"
                << "Parent path exists: " << fs::exists(filePath.parent_path())
                << "\n";
      // << "Permissions: "
      // << fs::status(filePath.parent_path()).permissions()
      // << std::endl;
      return false;
    }
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return false;
  }
}

std::string remove_file_prefix(const fs::path &path,
                               const fs::path &repo_prefix) {
  std::string path_str = path.string();
  std::string repo_prefix_str = repo_prefix.string();
  int idx = path_str.find(repo_prefix_str);
  if (idx != std::string::npos) {
    path_str.erase(0, repo_prefix_str.length());
  }
  return path_str;
}

// available values: git blob, or git tree
// reference:
// https://unix.stackexchange.com/questions/450480/file-permission-with-six-octal-digits-in-git-what-does-it-mean
std::string get_file_type(int mode) {
  int file_type = ((mode & 0xF000) >> 12);
  return file_type == 0x4 ? "tree" : "blob";
}

fs::perms get_unix_permissions(int mode) {
  std::string file_type = get_file_type(mode);
  // set default permission for directories
  if (file_type == "tree") {
    return fs::perms::owner_read | fs::perms::owner_write |
           fs::perms::owner_exec | fs::perms::group_read |
           fs::perms::group_exec | fs::perms::others_read |
           fs::perms::others_exec;
  }
  mode = mode & 0x1FF;
  fs::perms permissions = fs::perms::none;

  // Owner permissions
  if (mode & 0400)
    permissions |= fs::perms::owner_read;
  if (mode & 0200)
    permissions |= fs::perms::owner_write;
  if (mode & 0100)
    permissions |= fs::perms::owner_exec;

  // Group permissions
  if (mode & 0040)
    permissions |= fs::perms::group_read;
  if (mode & 0020)
    permissions |= fs::perms::group_write;
  if (mode & 0010)
    permissions |= fs::perms::group_exec;

  // Others permissions
  if (mode & 0004)
    permissions |= fs::perms::others_read;
  if (mode & 0002)
    permissions |= fs::perms::others_write;
  if (mode & 0001)
    permissions |= fs::perms::others_exec;

  return permissions;
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
