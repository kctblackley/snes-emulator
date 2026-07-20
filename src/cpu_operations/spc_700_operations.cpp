#include "spc_700_operations.hpp"
#include "spc_700_addressing_modes.hpp"

namespace SPC700Functions {
	void NOP(CPU& cpu, bool skipped) {
		return;
	}

	void IncrementPC(CPU& cpu, bool skipped) {
		cpu.regs.PC++;
	}

	void Next(CPU& cpu, bool skipped) {
		cpu.BufferOpCode = cpu.read(cpu.regs.PC);
	}

	void CLRC(CPU& cpu, bool skipped) { cpu.regs.P = (cpu.regs.P & ~0x01); }
	void SETC(CPU& cpu, bool skipped) { cpu.regs.P = (cpu.regs.P |  0x01); }
	void CLRP(CPU& cpu, bool skipped) { cpu.regs.P = (cpu.regs.P & ~0x20); }
	void SETP(CPU& cpu, bool skipped) { cpu.regs.P = (cpu.regs.P |  0x20); }
	void DI(CPU& cpu, bool skipped)   { cpu.regs.P = (cpu.regs.P & ~0x04); }
	void EI(CPU& cpu, bool skipped)   { cpu.regs.P = (cpu.regs.P |  0x04); }

	void CLRV(CPU& cpu, bool skipped) {
		cpu.clear_flag_V();
		cpu.clear_flag_H();
	}

	void NOTC(CPU& cpu, bool skipped) {
		if (cpu.get_flag_C() ^ 1) {
			cpu.set_flag_C();
		} else {
			cpu.clear_flag_C();
		}
	}
}

// NOP (00)
Instruction s_00 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// CLRP (20)
Instruction s_20 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::CLRP),
	MakeHandler(SPC700Functions::Next)
};

// SETP (40)
Instruction s_40 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::SETP),
	MakeHandler(SPC700Functions::Next)
};

// CLRC (60)
Instruction s_60 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::CLRC),
	MakeHandler(SPC700Functions::Next)
};

// SETC (80)
Instruction s_80 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::SETC),
	MakeHandler(SPC700Functions::Next)
};

// EI (A0)
Instruction s_a0 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::EI),
	MakeHandler(SPC700Functions::Next)
};

// DI (C0)
Instruction s_c0 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DI),
	MakeHandler(SPC700Functions::Next)
};

// CLRV (E0)
Instruction s_e0 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::CLRV),
	MakeHandler(SPC700Functions::Next)
};

// NOTC (ED)
Instruction s_ed = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOTC),
	MakeHandler(SPC700Functions::Next)
};

Instruction s_nop = {
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
};