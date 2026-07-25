#include "store.hpp"

class OpenBus : public Store {
public:
	Byte read(SNESAddress address) override {
		address_bus = address;
		return 0x00; // unused: Bus::read() takes the open-bus latch path instead, see is_not_open_bus()
	}

	void write(SNESAddress address, Byte value) override {
		address_bus = address;
		return;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return 0;
	}

	bool is_not_open_bus() const override {
		return false;
	}

private:
	SNESAddress address_bus;
};