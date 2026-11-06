#include "pack_util.h"
#include "repository.h"
#include "util.h"
#include <array>
#include <fstream>
#include <stdexcept>
#include <zlib.h>

namespace {
// zstr reads concatenated streams and accepts some truncated inputs. Git needs
// checksum validation and a stop at the first stream, particularly in
// packfiles.
std::string read_zlib_stream(std::ifstream &file) {
  if (!file) {
    throw std::runtime_error("Unable to read compressed object");
  }
  z_stream stream{};
  const int init_result = inflateInit(&stream);
  if (init_result != Z_OK) {
    throw std::runtime_error(zError(init_result));
  }
  struct Cleanup {
    z_stream &stream;
    ~Cleanup() { inflateEnd(&stream); }
  } cleanup{stream};

  std::array<unsigned char, 16384> input{}, output{};
  std::string result;
  while (true) {
    if (stream.avail_in == 0) {
      file.read(reinterpret_cast<char *>(input.data()), input.size());
      if (file.bad() || (file.fail() && !file.eof())) {
        throw std::runtime_error("Failed to read compressed object");
      }
      if (file.gcount() == 0) {
        throw std::runtime_error("Truncated zlib stream");
      }
      stream.next_in = input.data();
      stream.avail_in = static_cast<uInt>(file.gcount());
    }
    stream.next_out = output.data();
    stream.avail_out = output.size();
    const int status = inflate(&stream, Z_NO_FLUSH);
    if (status != Z_OK && status != Z_STREAM_END) {
      throw std::runtime_error("Zlib decompression error: " +
                               std::string(zError(status)));
    }
    result.append(reinterpret_cast<const char *>(output.data()),
                  output.size() - stream.avail_out);
    if (status == Z_STREAM_END) {
      return result;
    }
  }
}
} // namespace

// Recursively resolves loose refs, falling back to packed refs.
std::string resolve_ref(const fs::path &ref_path) {
  GitRepository repo = GitRepository::find();
  if (fs::exists(ref_path)) {
    const std::string ref = read_file(ref_path, true);
    if (ref.starts_with("ref: ")) {
      return resolve_ref(repo.repo_path(ref.substr(5)));
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
read_git_object_data(const std::string &sha) {
  GitRepository repo = GitRepository::find();
  fs::path file_path = repo.object_path(sha);
  fs::path paths = repo.file(file_path);

  if (!fs::is_regular_file(paths)) {
    throw std::runtime_error(paths.string() + " is not found");
  }
  std::ifstream input_file(paths.string(),
                           std::ios_base::in | std::ios_base::binary);
  std::string raw = read_zlib_stream(input_file);
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

  // The header size is uncompressed; zlib determines the compressed boundary.
  std::string payload = read_zlib_stream(file);
  if (payload.size() != object_size) {
    throw std::runtime_error("Malformed packed object: bad length");
  }
  return {type_to_string(object_type), std::move(payload)};
}
