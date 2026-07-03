#include "store.hpp"

class APUPorts : public Store {
public:

	Byte read(SNESAddress address) override {
		address_bus = address;
		return apu_ports[get_index(address)];
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		apu_ports[get_index(address)] = value;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return 0;
	}

private:
	std::array<Byte, APU_PORTS_SIZE> apu_ports {};
	SNESAddress address_bus;

	uint64_t get_index(SNESAddress address) {
		return address.offset - APU_PORTS_SECTION;
	}
};