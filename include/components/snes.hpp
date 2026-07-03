#pragma once

#include <string>

#include "rom_loader.hpp"

#include "bus.hpp"
#include "ricoh_5a22.hpp"
#include "common.hpp"

class SNES {
public:

	SNES();

	// How the ROM is loaded is left to main.cpp. SNES expects a cartridge to be in std::vector format.
	void load(const std::vector<Byte>& rom); // This ROM is routed through the bus and into the cartridge store
	void poll(); // Poll finds components that are due to be ticked for the current master cycle
	void tick_snes(); // Polls components, ticks components if there are any, then increments master cycle by 1
	void run();

	void reset();
	void initialise();

private:

	// SNES owns all of the components (code intends to mimic physical structure of SNES itself)
	std::unique_ptr<Bus> bus;

	// Devices vector
	std::vector<std::unique_ptr<Component>> devices;

	// Pointers to specific devices for ease of access
	Ricoh5A22* ricoh_5a22 = nullptr;
	
	// SNES acts as a scheduler that ticks all components
	CycleCount master_cycle;
};