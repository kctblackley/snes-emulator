#include "store.hpp"

class WRAM : public Store {
public:

	Byte read(SNESAddress address) override {
		address_bus = address;
		return wram[get_index(address)];
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		wram[get_index(address)] = value;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return WRAM_PENALTY;
	}

private:
	std::array<Byte, WRAM_SIZE> wram {};
	SNESAddress address_bus;

	uint64_t get_index(SNESAddress address) {
		if (address.bank == 0x7E || address.bank == 0x7F) {
			return address.offset + (WRAM_BANK_SIZE * (address.bank - 0x7E) );
		} else {
			return address.offset;
		}
	}
};