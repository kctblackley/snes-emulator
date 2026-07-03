#pragma once
#include "bus.hpp"

class Component {
public:
	
	virtual ~Component() = default;

	virtual void add_cycles(CycleCount cycles) = 0;

	virtual void tick_component() = 0;
	virtual CycleCount get_cycle() = 0;
	virtual TickCount get_tick() = 0;

	virtual void reset() {}
	virtual void initialise() {}
};