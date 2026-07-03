#include "snes.hpp"

SNES::SNES() : master_cycle(0) {
	// Create component and link to Bus

	bus = std::make_unique<Bus>();
	ricoh_5a22 = std::make_unique<Ricoh5A22>(bus.get());

	// Add wait-state-callback, when slow/fast data is accessed
	bus->set_wait_callback([this](CycleCount cycles) {
		ricoh_5a22->add_cycles(cycles);
	});
}

void SNES::load(const std::vector<Byte>& rom) {
	return;
}

void SNES::tick_snes() {
	master_cycle++;
}

void SNES::poll() {
	if (ricoh_5a22->get_cycle() == master_cycle) {
		ricoh_5a22->tick_component();
	}
}

void SNES::run() {
	bool running = true;

	while (running) {
		poll();
		tick_snes();

		// Temporary pre-mature stopper
		if (master_cycle >= MASTER_CLOCK) {
			running = false;
		}
	}

	std::cout << std::dec << (int)(ricoh_5a22->get_tick()) << std::endl;
}