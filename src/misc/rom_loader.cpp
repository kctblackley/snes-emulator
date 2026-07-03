#include "rom_loader.hpp"

std::vector<Byte> load_rom_file(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);

    return std::vector<std::uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}

std::vector<Byte> load_rom_text(const std::string& text) {
	std::istringstream iss(text);
    std::vector<std::uint8_t> data;

    std::string token;
    while (iss >> token)
    {
        unsigned int value = std::stoul(token, nullptr, 16);

        if (value > 0xFF)
            throw std::runtime_error("Byte value out of range");

        data.push_back(static_cast<std::uint8_t>(value));
    }

    return data;
}