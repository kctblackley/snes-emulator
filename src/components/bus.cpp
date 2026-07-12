#include "bus.hpp"

Bus::Bus() {
	open_bus = std::make_unique<OpenBus>();
	wram = std::make_unique<WRAM>();
	cartridge = std::make_unique<Cartridge>();

}

void Bus::set_wait_callback(WaitCallback callback) {
	this->callback = callback;
}

Store* Bus::system_area(SNESAddress address) {
	if (address.offset >= WRAM_SECTION && address.offset <= OPEN_BUS_SECTION) {
		return wram.get();
	}
	if (address.offset >= OPEN_BUS_SECTION && address.offset < PPU_PORTS_SECTION) {
		return open_bus.get();
	}
	if (address.offset >= PPU_PORTS_SECTION && address.offset < APU_PORTS_SECTION) {
		return open_bus.get(); // UNIMPLEMENTED
	}
	if (address.offset >= APU_PORTS_SECTION && address.offset < WRAM_ACCESS_SECTION) {
		return open_bus.get(); // UNIMPLEMENTED
	}
	if (address.offset >= WRAM_ACCESS_SECTION && address.offset < CPU_PORTS_SECTION) {
		return open_bus.get(); // UNIMPLEMENTED
	}
	if (address.offset >= CPU_PORTS_SECTION && address.offset < CPU_DMA_PORTS_SECTION) {
		return open_bus.get(); // UNIMPLEMENTED
	}
	if (address.offset >= CPU_DMA_PORTS_SECTION && address.offset < EXPANSION_DATA_SECTION) {
		return open_bus.get(); // UNIMPLEMENTED
	}
	if (address.offset >= EXPANSION_DATA_SECTION && address.offset < CARTRIDGE_SECTION) {
		return open_bus.get(); // UNIMPLEMENTED
	}
	if (address.offset >= CARTRIDGE_SECTION && address.offset <= MAX_OFFSET_SECTION) {
		return cartridge.get();
	}
	return open_bus.get();
}

Store* Bus::route(SNESAddress address) {
	Quadrant quadrant = get_quadrant(address.bank);
	switch(quadrant) {
	case 1:
		return system_area(address);
		break;
	case 2:
		if (address.bank == 0x7E || address.bank == 0x7F) {
			return wram.get();
		} else {
			return cartridge.get();
		}
		break;
	case 3:
		return system_area(address);
		break;
	case 4:
		return cartridge.get();
	default:
		return cartridge.get();
	}

	return open_bus.get();
}

void Bus::write(Address addr, Byte value) {
	if (test_mode) {
		test_memory[addr & 0xFFFFFF] = value;
		return;
	}

	SNESAddress address = split_address(addr);
	Store* store = route(address);
	
	if (store->is_not_open_bus()) {
		data_bus = value;
		store->write(address, value);
		callback(store->penalty());
	}
}

Byte Bus::read(Address addr) {
	if (test_mode) {
		auto it = test_memory.find(addr & 0xFFFFFF);
		return it != test_memory.end() ? it->second : 0x00;
	}

	SNESAddress address = split_address(addr);
	Store* store = route(address);

	if (store->is_not_open_bus()) {
		data_bus = store->read(address);
		callback(store->penalty());
	}

	return data_bus;
}

void Bus::load_cartridge(const std::string& directory) {
	cartridge->load_cartridge(directory);
}

void Bus::enable_test_mode() {
	test_mode = true;
}

void Bus::disable_test_mode() {
	test_mode = false;
	test_memory.clear();
}

void Bus::reset_test_memory() {
	test_memory.clear();
}

Byte Bus::test_peek(Address addr) {
	auto it = test_memory.find(addr & 0xFFFFFF);
	return it != test_memory.end() ? it->second : 0x00;
}

void Bus::test_poke(Address addr, Byte value) {
	test_memory[addr & 0xFFFFFF] = value;
}