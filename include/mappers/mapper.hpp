#pragma once
#include "common.hpp"
#include <vector>
#include <optional>

class Mapper {
public:
	virtual ~Mapper() = default;

	Byte read(SNESAddress address);
	void write(SNESAddress address, Byte value);

	virtual void to_string() = 0;

	void load_rom(std::vector<Byte> rom) {
		this->rom = rom;
	}

	void load_sram(Byte ram_size) {
		size_t sram_size = 0;
		if (ram_size != 0) {
			sram_size = (1024u << ram_size);
		}
		sram.assign(sram_size, 0);
	}

protected:
	virtual std::optional<Address> rom_idx(SNESAddress address) const = 0;
	virtual std::optional<Address> sram_idx(SNESAddress address) const = 0;

	void log_info() {
		std::cout << "ROM size: " << rom.size() << "\n";
		std::cout << "SRAM size: " << sram.size() << "\n";
	}

	std::vector<Byte> rom;
	std::vector<Byte> sram;
};