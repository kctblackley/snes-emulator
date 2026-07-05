#include "mapper.hpp"

class LoROM : public Mapper {
public:
	void to_string() override {
		std::cout << "LoROM\n";
		log_info();
	}

protected:
	std::optional<Address> rom_idx(SNESAddress address) const override {
		if (address.offset < 0x8000) {
			return std::nullopt;
		}

		return ((address.bank & 0x7F) << 15) | (address.offset & 0x7FFF);
	}

	std::optional<Address> sram_idx(SNESAddress address) const override {
		if (!(address.bank >= 0x70 && address.bank <= 0x7d)) {
			return std::nullopt;
		}

		if (address.offset >= 0x8000) {
			return std::nullopt;
		}

		return ((address.bank & 0x0F) << 15) | address.offset;
	}
};