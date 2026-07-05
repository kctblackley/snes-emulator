#pragma once
#include "cpu.hpp"
#include "ricoh_5a22_optable.hpp"

class Ricoh5A22 : public CPU {
public:
	explicit Ricoh5A22(Bus* bus);

	void add_cycles(CycleCount cycles) override;

	void run_half_cycle();
	void tick_component() override;
	CycleCount get_cycle() override;
	TickCount get_tick() override;

	void reset() override;
	void initialise() override;

	Byte read(Address addr) override {
		return bus->read(addr);
	}

	void write(Address addr, Byte value) override {
		bus->write(addr, value);
	}

private:
	Bus* bus = nullptr;

	CycleCount cycle; // cycle when Ricoh 5A22 is to be ticked as a whole
	CycleCount instruction_cycle; // for instructions themselves
	TickCount tick; // Important, these are half-ticks
};