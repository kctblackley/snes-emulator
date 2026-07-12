#include "ricoh_5a22.hpp"

Ricoh5A22::Ricoh5A22(Bus* bus) : bus(bus), cycle(0), instruction_cycle(0) {}

void Ricoh5A22::add_cycles(CycleCount cycles) {
	this->cycle += cycles;
}

CycleCount Ricoh5A22::get_cycle() {
	return this->cycle;
}

TickCount Ricoh5A22::get_tick() {
	return this->tick;
}

void Ricoh5A22::poll_interrupts() {
	return;
}

void Ricoh5A22::apply_invariants() {
	if (regs.emulation_mode) {
		regs.X = regs.X & 0x00FF;
		regs.Y = regs.Y & 0x00FF;
		regs.S = (regs.S & 0x00FF) | 0x0100;
	}
}

void Ricoh5A22::tick_multiply_divisor() {
	if (multiplier.half_cycles_since_init > 0) {
		multiplier.half_cycles_since_init--;
	}
	if (divisor.half_cycles_since_init > 0) {
		divisor.half_cycles_since_init--;
	}
}

void Ricoh5A22::run_half_cycle() {
	apply_invariants();
	log();
	tick_multiply_divisor();
	Opcode op = get_opcode(regs.emulation_mode ? emulation_optable : native_optable, BufferOpCode, instruction_cycle, *this);
	op.function(*this, op.skipped);
}

void Ricoh5A22::tick_component() { // when the component is ticked, it does a half tick in actuality
	tick++;
	if constexpr (!HALF_CYCLES) {
		run_half_cycle();
	}
	run_half_cycle();
	this->add_cycles(RICOH_5A22_CYCLE);
}

void Ricoh5A22::reset() {
	return;
}

void Ricoh5A22::initialise() {
	uint8_t lo = read(0x00FFFC);
	uint8_t hi = read(0x00FFFD);

	regs.PB = 0;
	regs.PC = (hi << 8) | lo;
	regs.P = 0x34;
	BufferOpCode = read(regs.PC);
	regs.emulation_mode = true;
	std::cout << "Initialised PC to " << regs.PC << "\n";
	
	cycle = 0;
	return;
}

void Ricoh5A22::log() {
	if (cycle % 1000000 != 0) {
		return;
	}
	std::cout << "PC:PB: " << std::hex << std::setw(2) << std::setfill('0') << (int)regs.PB << ":"
	                       << std::hex << std::setw(4) << std::setfill('0') << (int)regs.PC << " "
	          << "OP: "    << std::hex << std::setw(2) << std::setfill('0') << (int)BufferOpCode << " "
	          << "CYC: "   << std::dec << (int)instruction_cycle
	          << "\n";
}