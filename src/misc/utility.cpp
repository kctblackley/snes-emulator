#include "utility.hpp"

std::vector<Byte> load_rom(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);

    return std::vector<std::uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}