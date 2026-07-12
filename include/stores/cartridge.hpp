#pragma once
#include <algorithm>
#include "store.hpp"
#include "mapper.hpp"
#include "lorom_mapper.hpp"
#include "hirom_mapper.hpp"
#include "exhirom_mapper.hpp"


enum class MapperType {
	LoROM,
	HiROM,
	ExHiROM
};

struct CartridgeHeader {
	std::string title;

	Byte map_mode;
	Byte cartridge_type;

	Byte rom_size;
	Byte ram_size;

	Byte region;
	Byte version;

	Byte checksum;
	Byte complement;

	Byte reset_vector;
};

struct MapperCandidate {
	MapperType mapper;
	CartridgeHeader h;
	int score;
};

CartridgeHeader parse_header(const std::vector<Byte>& rom, size_t offset);
int score(const MapperCandidate& candidate, size_t rom_size);

class Cartridge : public Store {
public:

	Byte read(SNESAddress address) override {
		address_bus = address;
		return mapper->read(address);
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		mapper->write(address, value);
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return 0;
	}

	void load_cartridge(const std::string& directory) {
		std::vector<Byte> rom = load_rom(directory);

		std::vector<MapperCandidate> candidates;
		
		auto try_add = [&](MapperType type, size_t offset) {
	        if (offset + 0x40 <= rom.size()) {   // header needs up to offset+0x3e
	            candidates.push_back({type, parse_header(rom, offset), 0});
	        }
	    };

	    try_add(MapperType::LoROM,   0x7fc0);
	    try_add(MapperType::HiROM,   0xffc0);
	    try_add(MapperType::ExHiROM, 0x40ffc0);

	    if (candidates.empty()) {
	        throw std::runtime_error("ROM too small to contain a valid header");
	    }

		for (auto& c : candidates) {
			c.score = score(c, rom.size());
		}

		auto best = std::max_element(
			candidates.begin(),
			candidates.end(),
			[](const auto& a, const auto& b) {
				return a.score < b.score;
			}
		);

		switch (best->mapper) {
		case MapperType::LoROM:
			mapper = std::make_unique<LoROM>();
			break;
		case MapperType::HiROM:
			mapper = std::make_unique<HiROM>();
			break;
		case MapperType::ExHiROM:
			mapper = std::make_unique<ExHiROM>();
			break;
		}

		header = best->h;
		mapper->load_rom(rom);
		mapper->load_sram(header.ram_size);
		mapper->to_string();
		std::cout << header.title << "\n";
	}

private:
	Byte mapping; // Stores cartridge's mapping

	CartridgeHeader header;
	std::unique_ptr<Mapper> mapper;

	SNESAddress address_bus;
};