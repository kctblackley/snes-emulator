#include "snes.hpp"

SNES::SNES() : master_cycle(0) {
	// Create component and link to Bus

	bus = std::make_unique<Bus>();
	
	devices.push_back(std::make_unique<Ricoh5A22>(bus.get()));
	// Do not push SPC700 as the SNES does not tick that itself (separate component)
	
	ricoh_5a22 = static_cast<Ricoh5A22*>(devices[0].get());

	spc_700 = std::make_unique<SPC700>();

	bus->connect_cpu(ricoh_5a22);
	bus->connect_apu(spc_700.get());

	// Add wait-state-callback, when slow/fast data is accessed
	bus->set_wait_callback([this](CycleCount cycles) {
		ricoh_5a22->add_cycles(cycles);
	});
}

void SNES::load_cartridge(const std::string& directory) {
	bus->load_cartridge(directory);
	initialise();
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

	auto start = std::chrono::high_resolution_clock::now();
	
	while (running) {
		tick_snes();

		// Temporary pre-mature stopper
		if (master_cycle >= MASTER_CLOCK) {
			running = false;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time taken: " << elapsed.count() << " seconds\n";
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
