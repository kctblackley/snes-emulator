#include "snes.hpp"
#include <chrono>

SNES::SNES() : master_cycle(0) {
	// Create component and link to Bus

	bus = std::make_unique<Bus>();
	
	devices.push_back(std::make_unique<Ricoh5A22>(bus.get()));
	devices.push_back(std::make_unique<PPU>(bus.get()));
	
	// Do not push SPC700 as the SNES does not tick that itself (separate component)
	
	ricoh_5a22 = static_cast<Ricoh5A22*>(devices[0].get());
	ppu = static_cast<PPU*>(devices[1].get());

	spc_700 = std::make_unique<SPC700>();
	renderer = std::make_unique<Renderer>();
	dma_controller = std::make_unique<DMAController>();

	ricoh_5a22->connect_renderer(renderer.get());
	ricoh_5a22->connect_ppu(ppu);

	bus->connect_cpu(ricoh_5a22);
	bus->connect_apu(spc_700.get());
	bus->connect_dma_controller(dma_controller.get());
	bus->connect_ppu(ppu);

	dma_controller->connect_cpu(ricoh_5a22);
	dma_controller->connect_bus(bus.get());

	ppu->connect_dma_controller(dma_controller.get());
	ppu->connect_bus(bus.get());
	ppu->connect_renderer(renderer.get());
	ppu->create_window();

	// Add wait-state-callback, when slow/fast data is accessed
	bus->set_wait_callback([this](CycleCount cycles) {
		ricoh_5a22->add_cycles(cycles);
	});
}

void SNES::load_cartridge(const std::string& directory) {
	bus->load_cartridge(directory);
	initialise();
	bus->connect_cpu_to_cartridge(ricoh_5a22);
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
	int seconds_to_run = 200;
	int seconds = 0;
	int total_ticks = 0;

	auto fps_timer = std::chrono::steady_clock::now();
	int fps_frames = 0;

	while (running) {
		CycleCount prev_master_cycle = master_cycle;
		tick_snes();

		if (ppu->frame_finished) {
			ppu->push_framebuffer();
			renderer->loop();
			ppu->frame_finished = false;

			fps_frames++;

			auto now = std::chrono::steady_clock::now();
	        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
	            now - fps_timer
	        );

	        if (elapsed.count() >= 1000) {
	            double fps = fps_frames * 1000.0 / elapsed.count();

	            std::cout << "\rFPS: " << fps << std::flush;

	            fps_frames = 0;
	            fps_timer = now;
	        }
		}

		total_ticks += 1;

		running = renderer->running;
	}

	renderer->close_window();
	
	auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time taken: " << elapsed.count() << " seconds\n";
	std::cout << std::dec << (int)(ricoh_5a22->get_tick()) << std::endl;
}

void SNES::reset() {
	for (const auto& d : devices) {
		d->reset();
	}
	dma_controller->reset();
}

void SNES::initialise() {
	for (const auto& d : devices) {
		d->initialise();
	}
	dma_controller->initialise();
}

