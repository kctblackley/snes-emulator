#include "spc_700.hpp"

// Need to create APU bus mechanics!
SPC700::SPC700() : cycle(0), instruction_cycle(0) {}

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
	return;
}

void SPC700::tick_component() { // when the component is ticked, it does a half tick in actuality
	return;
}

void SPC700::reset() {
	return;
}

void SPC700::initialise() {
	return;
}