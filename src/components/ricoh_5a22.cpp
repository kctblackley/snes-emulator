#include "ricoh_5a22.hpp"

Ricoh5A22::Ricoh5A22(Bus* bus) : bus(bus), cycle(0) {}

void Ricoh5A22::add_cycles(CycleCount cycles) {
	this->cycle += cycles;
}

CycleCount Ricoh5A22::get_cycle() {
	return this->cycle;
}

TickCount Ricoh5A22::get_tick() {
	return this->tick;
}

void Ricoh5A22::tick_component() { // when the component is ticked, it does a half tick in actuality
	tick++;
	bus->read(0x7FFFFF);
	this->add_cycles(RICOH_5A22_CYCLE);
}

void Ricoh5A22::reset() {
	return;
}

void Ricoh5A22::initialise() {
	return;
}
