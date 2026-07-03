#pragma once

#include "common.hpp"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>

// For actual ROM files
std::vector<Byte> load_rom_file(const std::string& filename);

// For text, e.g. of raw text 3e 4f 5a etc.

// For the SSTs
std::vector<Byte> load_rom_text(const std::string& text);