#include "store.hpp"

class PPUPorts : public Store {
public:

	Byte read(SNESAddress address) override {
		address_bus = address;
		return ppu_ports[get_index(address)];
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		ppu_ports[get_index(address)] = value;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return 0;
	}

private:
	std::array<Byte, PPU_PORTS_SIZE> ppu_ports {};
	SNESAddress address_bus;

	uint64_t get_index(SNESAddress address) {
		return address.offset - PPU_PORTS_SECTION;
	}
};