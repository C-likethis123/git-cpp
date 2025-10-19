#include "pack_util.h"
#include "util.h"
#include <cstdint>
#include <fstream>

std::pair<uint8_t, uint64_t> read_pack_object_header(std::ifstream &file_pos) {
  // Read the first byte to get type and initial size
  std::string first_byte = read_bytes(file_pos, 1);
  uint8_t byte = static_cast<uint8_t>(first_byte[0]);

  // Extract object type from bits 7-4
  uint8_t object_type = (byte >> 4) & 0x0F;

  // Extract initial size from bits 3-0
  uint64_t size = byte & 0x0F;

  // If the MSB is set, we need to read more bytes for the size
  int shift = 4;
  while (byte & 0x80) {
    first_byte = read_bytes(file_pos, 1);
    byte = static_cast<uint8_t>(first_byte[0]);
    size |= ((byte & 0x7F) << shift);
    shift += 7;
  }

  return {object_type, size};
}

std::string type_to_string(uint8_t type) {
  switch (type) {
  case 1:
    return "commit";
  case 2:
    return "tree";
  case 3:
    return "blob";
  case 4:
    return "tag";
  case 6:
    return "ofs-delta";
  case 7:
    return "ref-delta";
  default:
    throw std::runtime_error("Unknown object type: " + std::to_string(type));
  }
}
