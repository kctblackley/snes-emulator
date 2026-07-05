#pragma once
#include <functional>
#include "common.hpp"

#include "open_bus.hpp"
#include "wram.hpp"
#include "ppu_ports.hpp"
#include "apu_ports.hpp"
#include "wram_access.hpp"
#include "cpu_ports.hpp"
#include "cpu_dma_ports.hpp"
#include "expansion_data.hpp"
#include "cartridge.hpp"

class Bus {
public:
	// Lambda used as callback to prevent circular dependencies but allow bus to inflict penalties due to RAM/ROM speeds
	using WaitCallback = std::function<void(CycleCount cycles)>;

	Bus();

	// Lambda is passed in here to allow for this
	void set_wait_callback(WaitCallback callback);

	// Abstracts behaviour to find storage device that is to respond to read/write to a given address
	Store* system_area(SNESAddress address); 
	Store* route(SNESAddress address);

	// Read and write behaviours, abstracted as above
	void write(Address addr, Byte value);
	Byte read(Address addr);

	void load_cartridge(const std::string& directory);

private:
	WaitCallback callback;
	Byte data_bus;

	// RAII used as the bus owns all of the stores
	std::unique_ptr<OpenBus> open_bus;
	std::unique_ptr<WRAM> wram;

	std::unique_ptr<PPUPorts> ppu_ports;
	std::unique_ptr<APUPorts> apu_ports;
	std::unique_ptr<WRAMAccess> wram_access;
	std::unique_ptr<CPUPorts> cpu_ports;
	std::unique_ptr<CPUDMAPorts> cpu_dma_ports;
	std::unique_ptr<ExpansionData> expansion_data;
	std::unique_ptr<Cartridge> cartridge;
};