#include "mapper.hpp"

class HiROM : public Mapper {
public:
	void to_string() override {
		std::cout << "HiROM\n";
		log_info();
	}

protected:
	std::optional<Address> rom_idx(SNESAddress address) const override {
		if ((address.bank >= 0x40 && address.bank <= 0x7D) || address.bank >= 0xC0) {
			return ((address.bank & 0x3F) << 16) | address.offset;
		}

		if (address.offset >= 0x8000) {
			return ((address.bank & 0x3F) << 16) | address.offset;
		}

		return std::nullopt;
	}

	std::optional<Address> sram_idx(SNESAddress address) const override {
		if (!( (address.bank >= 0x20 && address.bank <= 0x3F) || (address.bank >= 0xA0 && address.bank <= 0xBF) )) {
			return std::nullopt;
		}

		if (address.offset < 0x6000 || address.offset > 0x7FFF) {
			return std::nullopt;
		}

		return ((address.bank & 0x1F) << 13) | (address.offset & 0x1FFF);
	}
};