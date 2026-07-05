#include "store.hpp"

class CPUPorts : public Store {
public:

	Byte read(SNESAddress address) override {
		address_bus = address;
		return cpu_ports[get_index(address)];
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		cpu_ports[get_index(address)] = value;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		if (address_bus.offset >= CPU_PORTS_SECTION && address_bus.offset < CPU_PORTS_NON_PENALTY_SECTION) {
			return CPU_PORTS_PENALTY;
		}
		return 0;
	}

private:
	std::array<Byte, CPU_PORTS_SIZE> cpu_ports {};
	SNESAddress address_bus;

	uint64_t get_index(SNESAddress address) {
		return address.offset - CPU_PORTS_SECTION;
	}
};