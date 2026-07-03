#pragma once
#include "component.hpp"

class Ricoh5A22 : public Component {
public:
	explicit Ricoh5A22(Bus* bus);

	void add_cycles(CycleCount cycles) override;

	void tick_component() override;
	CycleCount get_cycle() override;
	TickCount get_tick() override;

	void reset() override;
	void initialise() override;

private:
	Bus* bus = nullptr;

	CycleCount cycle;
	TickCount tick; // Important, these are half-ticks
};