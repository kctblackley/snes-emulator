#include "store.hpp"

class WRAMAccess : public Store {
public:

	Byte read(SNESAddress address) override {
		address_bus = address;
		return wram_access[get_index(address)];
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		wram_access[get_index(address)] = value;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return 0;
	}

private:
	std::array<Byte, WRAM_ACCESS_SIZE> wram_access {};
	SNESAddress address_bus;

	uint64_t get_index(SNESAddress address) {
		return address.offset - WRAM_ACCESS_SECTION;
	}
};