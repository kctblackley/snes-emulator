#include "spc_700.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdlib>

// Need to create APU bus mechanics!
SPC700::SPC700() : cycle(0), instruction_cycle(0) {
	std::cout << "Loading SPC700 IPL ROM\n";

	std::ifstream file("ipl/ipl.rom", std::ios::binary);

	if (!file) {
		std::cerr << "Missing IPL ROM.\n";
		std::cerr << "Please provide the IPL ROM, named 'ipl.rom', in the folder 'ipl'.\n";
		std::cerr << "Refer to README.md for instructions.\n";
		std::exit(EXIT_FAILURE);
	}

	file.read(
		reinterpret_cast<char*>(ipl_rom.data()),
		ipl_rom.size()
	);

	if (file.gcount() != 64) {
		std::cerr << "The IPL ROM provided is of the incorrect size.\n";
		std::cerr << "It must be 64 bytes in size to be valid.\n";
		std::cerr << "Refer to README.md for instructions.\n";
		std::exit(EXIT_FAILURE);
	}

	std::cout << "Loaded SPC700 IPL ROM\n";

	bus = std::make_unique<APUBus>();
	initialise();
}

void SPC700::add_cycles(CycleCount cycles) {
	this->cycle += cycles;
}

CycleCount SPC700::get_cycle() {
	return this->cycle;
}

TickCount SPC700::get_tick() {
	return this->tick;
}

void SPC700::poll_interrupts() {
	return;
}

void SPC700::apply_invariants() {
	return;
}

void SPC700::run_half_cycle() {
	Opcode op = get_opcode(optable, BufferOpCode, instruction_cycle, *this);
	op.function(*this, op.skipped);
}

void SPC700::accumulate(CycleCount delta) {
	accumulated_cycles += (double)delta;
	while (accumulated_cycles > SPC_700_CYCLE_CONSTANT) {
		if constexpr (SHOW_LOGS) {
			if (instruction_cycle == 0) {
				std::cout << "INSTRUCTION SPC700 PC=" << std::hex << (int)(regs.PC) << 
				             "OPCODE=" << std::hex << (int)(BufferOpCode) << std::endl;
				std::cout << "PC=" << std::hex << regs.PC
				    << " Y=" << std::hex << (int)regs.Y
				    << " P=" << std::hex << (int)regs.P
				    << "\n";
			}
		}
		tick_component();
		accumulated_cycles -= SPC_700_CYCLE_CONSTANT;
	}
}

void SPC700::tick_component() { // when the component is ticked, it does a half tick in actuality
	tick_timer(0, 128);
	tick_timer(1, 128);
	tick_timer(2, 16);
	tick++;
	if constexpr (!HALF_CYCLES) {
		run_half_cycle();
	}
	run_half_cycle();
}

void SPC700::reset() { // RUN IPL ROM HERE! MEMORY MAP THE IPL ROM!
	regs.A = 0x00;
	regs.X = 0x00;
	regs.Y = 0x00;
	regs.S = 0xEF;
	regs.P = 0x00;
	regs.PC = 0xFFC0;
	accumulated_cycles = 0;
	BufferOpCode = read(regs.PC);
	write(0xF0, 0x0A);
}

void SPC700::initialise() {
	reset();
}