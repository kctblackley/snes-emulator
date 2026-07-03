#pragma once

#include <string>

#include "bus.hpp"
#include "ricoh_5a22.hpp"
#include "common.hpp"

class SNES {
public:

	SNES();
	void load(const std::vector<Byte>& rom);
	void poll();
	void tick_snes();
	void run();

private:
	std::unique_ptr<Bus> bus;
	std::unique_ptr<Ricoh5A22> ricoh_5a22;

	CycleCount master_cycle;
};