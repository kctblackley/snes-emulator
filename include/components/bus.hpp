#pragma once
#include <functional>
#include <unordered_map>
#include "common.hpp"

#include "open_bus.hpp"
#include "wram.hpp"
#include "cartridge.hpp"

class Bus {
public:
	using WaitCallback = std::function<void(CycleCount cycles)>;

	Bus();

	void set_wait_callback(WaitCallback callback);

	Store* system_area(SNESAddress address); 
	Store* route(SNESAddress address);

	void write(Address addr, Byte value);
	Byte read(Address addr);

	void load_cartridge(const std::string& directory);

	void enable_test_mode();
	void disable_test_mode();
	void reset_test_memory();
	Byte test_peek(Address addr);
	void test_poke(Address addr, Byte value);

private:
	WaitCallback callback;
	Byte data_bus;

	bool test_mode = false;
	std::unordered_map<Address, Byte> test_memory;

	std::unique_ptr<OpenBus> open_bus;
	std::unique_ptr<WRAM> wram;

	std::unique_ptr<Cartridge> cartridge;
};