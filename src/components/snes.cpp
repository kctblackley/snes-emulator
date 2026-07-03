#include "snes.hpp"

SNES::SNES() : master_cycle(0) {
	// Create component and link to Bus

	bus = std::make_unique<Bus>();
	
	devices.push_back(std::make_unique<Ricoh5A22>(bus.get()));

	ricoh_5a22 = static_cast<Ricoh5A22*>(devices[0].get());

	// Add wait-state-callback, when slow/fast data is accessed
	bus->set_wait_callback([this](CycleCount cycles) {
		ricoh_5a22->add_cycles(cycles);
	});
}

void SNES::load(const std::vector<Byte>& rom) {
	bus->load_cartridge(rom);
}

void SNES::tick_snes() {
	auto next_device = std::min_element(
		devices.begin(), devices.end(),
		[](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) {
			return a->get_cycle() < b->get_cycle();
		}	
	);

	(*next_device)->tick_component();
	master_cycle = (*next_device)->get_cycle();
}

void SNES::poll() {
	if (ricoh_5a22->get_cycle() == master_cycle) {
		ricoh_5a22->tick_component();
	}
}

void SNES::run() {
	bool running = true;

	while (running) {
		tick_snes();

		// Temporary pre-mature stopper
		if (master_cycle >= MASTER_CLOCK) {
			running = false;
		}
	}

	std::cout << std::dec << (int)(ricoh_5a22->get_tick()) << std::endl;
}

void SNES::reset() {
	for (const auto& d : devices) {
		d->reset();
	}
}

void SNES::initialise() {
		for (const auto& d : devices) {
		d->initialise();
	}
}
