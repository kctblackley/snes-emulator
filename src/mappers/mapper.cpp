#include "mapper.hpp"

Byte Mapper::read(SNESAddress address) {
	if (auto idx = rom_idx(address)) {
		if (*idx >= rom.size()) {
			std::cerr << "ROM access out of range: " << std::hex << *idx << '\n';
		}
		return rom[*idx];
	}

	if (auto idx = sram_idx(address)) {
		if (*idx >= sram.size()) {
			std::cerr << "SRAM access out of range: " << std::hex << *idx << '\n';
		}
		return sram[*idx];
	}

	return 0xFF; // Might need to change this to consider open bus!
}

void Mapper::write(SNESAddress address, Byte value) {
	if (auto idx = sram_idx(address)) {
		sram[*idx] = value;
	}
}