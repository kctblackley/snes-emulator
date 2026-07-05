#include "store.hpp"

class ExpansionData : public Store {
public:

	Byte read(SNESAddress address) override {
		address_bus = address;
		return expansion_data[get_index(address)];
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		expansion_data[get_index(address)] = value;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return EXPANSION_DATA_PENALTY;
	}

private:
	std::array<Byte, EXPANSION_DATA_SIZE> expansion_data {};
	SNESAddress address_bus;

	uint64_t get_index(SNESAddress address) {
		return address.offset - EXPANSION_DATA_SECTION;
	}
};