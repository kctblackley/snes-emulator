#include "spc_700.hpp"

// Need to create APU bus mechanics!
SPC700::SPC700() : cycle(0), instruction_cycle(0) {
	bus = std::make_unique<APUBus>();
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

void SPC700::tick_component() { // when the component is ticked, it does a half tick in actuality
	tick++;
	if constexpr (!HALF_CYCLES) {
		run_half_cycle();
	}
	run_half_cycle();
	// this->add_cycles(RICOH_5A22_CYCLE); (THIS WILL WORK DIFFERENTLY)
}

void SPC700::reset() { // RUN IPL ROM HERE! MEMORY MAP THE IPL ROM!
	return;
}

void SPC700::initialise() {
	return;
}