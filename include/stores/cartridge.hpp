#include "store.hpp"

class Cartridge : public Store {
public:

	Byte read(SNESAddress address) override {
		address_bus = address;
		return cartridge[get_index(address)];
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		cartridge[get_index(address)] = value;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return 0;
	}

private:
	std::vector<Byte> cartridge {};
	SNESAddress address_bus;

	uint64_t get_index(SNESAddress address) {
		return 0; // More complicated, to do with LoRom and HiRom
	}
};