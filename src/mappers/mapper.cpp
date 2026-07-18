#include "mapper.hpp"
#include "ricoh_5a22.hpp"

Byte Mapper::read(SNESAddress address) {
	if (auto idx = rom_idx(address)) {
		return rom[(*idx) & (rom.size() - 1)];
	}

	if (auto idx = sram_idx(address)) {
		return sram[(*idx) & (sram.size() - 1)];
	}

	return 0x00; // Might need to change this to consider open bus!
}

void Mapper::write(SNESAddress address, Byte value) {
	if (auto idx = sram_idx(address)) {
		sram[(*idx) & (sram.size() - 1)] = value;
	}
}