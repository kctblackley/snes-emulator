#pragma once

#include <cstdint>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>
#include <iostream>

using Byte = uint8_t;

// For actual ROM files
std::vector<Byte> load_rom(const std::string& filename);

// For text, e.g. of raw text 3e 4f 5a etc.