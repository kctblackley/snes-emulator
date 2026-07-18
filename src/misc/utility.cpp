#include "utility.hpp"


constexpr size_t COPIER_HEADER_SIZE = 512;

bool has_copier_header(size_t file_size) {
    // Real SNES ROM images are always a multiple of 32KB (0x8000),
    // since that's the smallest bank size. If the file is exactly
    // COPIER_HEADER_SIZE bytes larger than a clean multiple, those
    // extra leading bytes are a copier header, not ROM data.
    if (file_size <= COPIER_HEADER_SIZE) {
        return false;
    }
    return ((file_size - COPIER_HEADER_SIZE) % 0x8000) == 0;
}

std::vector<Byte> load_rom(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    std::vector<Byte> rom{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()};

    if (has_copier_header(rom.size())) {
        std::cout << "Detected " << COPIER_HEADER_SIZE
                   << "-byte copier header, skipping it\n";
        rom.erase(rom.begin(), rom.begin() + COPIER_HEADER_SIZE);
    }

    return rom;
}