#ifndef PACK_UTIL_H
#define PACK_UTIL_H

#include <fstream>

std::pair<uint8_t, uint64_t> read_pack_object_header(std::ifstream &file_pos);
std::string type_to_string(uint8_t type);

#endif // PACK_UTIL_H
