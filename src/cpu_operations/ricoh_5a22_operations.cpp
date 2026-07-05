#include "ricoh_5a22_operations.hpp"

// Micro-instructions
// Functions are all of the type

namespace Ricoh5A22Functions {
	void NOP(CPU& cpu, bool skipped) {
		return;
	}

	void IncrementPC(CPU& cpu, bool skipped) {
		cpu.regs.PC++;
	}

	void Read(CPU& cpu, bool skipped) {
		cpu.read(0x7F0000);
	}
}

namespace Ricoh5A22Predicates {
	bool Never(CPU& cpu) {
		return false;
	}

	bool Even(CPU& cpu) {
		return ( (cpu.regs.PC & 0b1) == 0);
	}

	bool Odd(CPU& cpu) {
		return ( (cpu.regs.PC & 0b1) == 1);
	}
}


Instruction lda_a1 = {
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::Even),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
};

Instruction nop = {
	MakeHandler(Ricoh5A22Functions::Read, Ricoh5A22Predicates::Even),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
};