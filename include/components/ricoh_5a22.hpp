#pragma once
#include "bus.hpp"

class Ricoh5A22 {
public:
	Ricoh5A22(Bus* bus);

	void add_cycles(CycleCount cycles);

	void tick_component();
	CycleCount get_cycle();
	TickCount get_tick();

private:
	Bus* bus = nullptr;

	CycleCount cycle;
	TickCount tick; // Important, these are half-ticks
};