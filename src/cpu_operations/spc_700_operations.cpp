#include "spc_700_operations.hpp"
#include "spc_700_addressing_modes.hpp"

namespace SPC700Predicates {
	bool NoJump(CPU& cpu) {
		return !cpu.BufferJump;
	}
}

namespace SPC700Functions {
	Word ya(CPU& cpu) {
		return ((uint8_t)(cpu.regs.Y) << 8) | get_lo(cpu.regs.A);
	}

	void SetNZ(CPU& cpu, bool skipped, Word value) {
		if (value & 0x80) {
			cpu.set_flag_N();
		} else {
			cpu.clear_flag_N();
		}
		if (value == 0) {
			cpu.set_flag_Z();
		} else {
			cpu.clear_flag_Z();
		}
	}

	void SetFuncOperand(CPU& cpu, bool skipped) {
		cpu.BufferAddress = (cpu.BufferOperand | ((cpu.regs.P & 0x20) << 3));
	}

	void SetFuncX(CPU& cpu, bool skipped) {
		cpu.BufferAddress = (cpu.regs.X | ((cpu.regs.P & 0x20) << 3));
	}

	void sub_func(CPU& cpu, bool skipped, SubFunc func) {
		switch(func) {
		case SubFunc::ClearISetX:
			cpu.clear_flag_I();
			cpu.set_flag_X();
			break;
		case SubFunc::SetNZFlagRegisterA:
			SPC700Functions::SetNZ(cpu, skipped, cpu.regs.A);
			break;
		case SubFunc::SetNZFlagRegisterYA:
			SPC700Functions::SetNZ(cpu, skipped, cpu.regs.Y);
			break;
		case SubFunc::SetNZFlagRegisterX:
			SPC700Functions::SetNZ(cpu, skipped, cpu.regs.X);
			break;
		case SubFunc::SetNZFlagOperand:
			SPC700Functions::SetNZ(cpu, skipped, (uint8_t)cpu.BufferOperand);
			break;
		case SubFunc::SetSubFunc:
			SPC700Functions::SetFuncOperand(cpu, skipped);
			break;
		}
	}

	void NOP(CPU& cpu, bool skipped) {
		return;
	}

	template <SubFunc func = SubFunc::None>
	void IncrementPC(CPU& cpu, bool skipped) {
		cpu.regs.PC++;
		sub_func(cpu, skipped, func);
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

	template <typename From, typename To>
	void Read(CPU& cpu, bool skipped) {
		Word address;

		// read from...
		if constexpr (std::is_same_v<From, ReadFrom::PC>)     { address = cpu.regs.PC; }
		
		if constexpr (std::is_same_v<From, ReadFrom::StackMinus2>) { address = 0x0100 | (uint8_t)(cpu.regs.S - 2); }
		if constexpr (std::is_same_v<From, ReadFrom::StackMinus1>) { address = 0x0100 | (uint8_t)(cpu.regs.S - 1); }
		if constexpr (std::is_same_v<From, ReadFrom::Stack0>)      { address = 0x0100 | (uint8_t)(cpu.regs.S + 0); }
		if constexpr (std::is_same_v<From, ReadFrom::Stack1>)      { address = 0x0100 | (uint8_t)(cpu.regs.S + 1); }
		if constexpr (std::is_same_v<From, ReadFrom::Stack2>)      { address = 0x0100 | (uint8_t)(cpu.regs.S + 2); }

		if constexpr (std::is_same_v<From, ReadFrom::FFDE>) { address = 0xFFDE; }
		if constexpr (std::is_same_v<From, ReadFrom::FFDF>) { address = 0xFFDF; }

		if constexpr (std::is_same_v<From, ReadFrom::Address>) { address = cpu.BufferAddress; }

		Byte value = cpu.read(address);
		// read to...
		if constexpr (std::is_same_v<To, ReadTo::Discard>) { return; }
		if constexpr (std::is_same_v<To, ReadTo::P>)       { cpu.regs.P  = value; }
		if constexpr (std::is_same_v<To, ReadTo::A>)       { cpu.regs.A  = value; }
		if constexpr (std::is_same_v<To, ReadTo::X>)       { cpu.regs.X  = value; }
		if constexpr (std::is_same_v<To, ReadTo::Y>)       { cpu.regs.Y  = value; }
		if constexpr (std::is_same_v<To, ReadTo::PC>)      { cpu.regs.PC = value; }
		if constexpr (std::is_same_v<To, ReadTo::PCLow>)   { cpu.regs.PC = (get_hi(cpu.regs.PC) << 8) | (uint8_t)(value); }
		if constexpr (std::is_same_v<To, ReadTo::PCHigh>)  { cpu.regs.PC = ((uint8_t)value << 8) | get_lo(cpu.regs.PC); }
		
		if constexpr (std::is_same_v<To, ReadTo::Operand>)  { cpu.BufferOperand  = value; }
		if constexpr (std::is_same_v<To, ReadTo::Operand0>) { cpu.BufferOperand0 = value; }
		if constexpr (std::is_same_v<To, ReadTo::Operand1>) { cpu.BufferOperand1 = value; }
	}

	template <typename Value, typename To>
	void Write(CPU& cpu, bool skipped) {
		// write the value...
		Byte value;
		if constexpr (std::is_same_v<Value, WriteValue::P>)      { value = cpu.regs.P; }
		if constexpr (std::is_same_v<Value, WriteValue::A>)      { value = cpu.regs.A; }
		if constexpr (std::is_same_v<Value, WriteValue::X>)      { value = cpu.regs.X; }
		if constexpr (std::is_same_v<Value, WriteValue::Y>)      { value = cpu.regs.Y; }
		if constexpr (std::is_same_v<Value, WriteValue::PC>)     { value = cpu.regs.PC; }
		if constexpr (std::is_same_v<Value, WriteValue::PCHigh>) { value = get_hi(cpu.regs.PC); }
		if constexpr (std::is_same_v<Value, WriteValue::PCLow>)  { value = get_lo(cpu.regs.PC); }

		if constexpr (std::is_same_v<Value, WriteValue::Operand>)   { value = cpu.BufferOperand; }
		if constexpr (std::is_same_v<Value, WriteValue::Operand0>)  { value = cpu.BufferOperand0; }
		if constexpr (std::is_same_v<Value, WriteValue::Operand1>)  { value = cpu.BufferOperand1; }

		Word address;
		if constexpr (std::is_same_v<To, WriteTo::StackMinus2>) { address = 0x0100 | (uint8_t)(cpu.regs.S - 2); }
		if constexpr (std::is_same_v<To, WriteTo::StackMinus1>) { address = 0x0100 | (uint8_t)(cpu.regs.S - 1); }
		if constexpr (std::is_same_v<To, WriteTo::Stack0>)      { address = 0x0100 | (uint8_t)(cpu.regs.S + 0); }
		if constexpr (std::is_same_v<To, WriteTo::Stack1>)      { address = 0x0100 | (uint8_t)(cpu.regs.S + 1); }
		if constexpr (std::is_same_v<To, WriteTo::Stack2>)      { address = 0x0100 | (uint8_t)(cpu.regs.S + 2); }
		if constexpr (std::is_same_v<To, WriteTo::Address>)     { address = cpu.BufferAddress; }
		
		// likely to expand to more stack + ...

		cpu.write(address, value);
	}

	template <int value = 1, SubFunc func = SubFunc::None>
	void DecrementS(CPU& cpu, bool skipped) {
		cpu.regs.S = (uint8_t)(cpu.regs.S - value);
		SPC700Functions::sub_func(cpu, skipped, func);
	}

	template <int value = 1, SubFunc func = SubFunc::None>
	void IncrementS(CPU& cpu, bool skipped) {
		cpu.regs.S = (uint8_t)(cpu.regs.S + value);
		SPC700Functions::sub_func(cpu, skipped, func);
	}

	template <int value = 1, SubFunc func = SubFunc::None>
	void DecrementX(CPU& cpu, bool skipped) {
		cpu.regs.X = (uint8_t)(cpu.regs.X - value);
		SPC700Functions::sub_func(cpu, skipped, func);
	}

	template <int value = 1, SubFunc func = SubFunc::None>
	void IncrementX(CPU& cpu, bool skipped) {
		cpu.regs.X = (uint8_t)(cpu.regs.X + value);
		SPC700Functions::sub_func(cpu, skipped, func);
	}

	template<int call = 0>
	void TCallLow(CPU& cpu, bool skipped) {
		Byte value = cpu.read(0xFFDE - (2 * call));
		cpu.regs.PC = (get_hi(cpu.regs.PC) << 8) | value;
	}

	template<int call = 0>
	void TCallHigh(CPU& cpu, bool skipped) {
		Byte value = cpu.read(0xFFDF - (2 * call));
		cpu.regs.PC = (value << 8) | get_lo(cpu.regs.PC);
	}

	template <int step = 1>
	void DAA(CPU& cpu, bool skipped) {
		switch(step) {
		case 1:
			if (cpu.get_flag_C() || cpu.regs.A > 0x99) {
				cpu.regs.A = (uint8_t)(cpu.regs.A + 0x60);
				cpu.set_flag_C();
			}
			break;
		case 2:
			if (cpu.get_flag_H() || (cpu.regs.A & 0x0F) > 0x09) {
				cpu.regs.A = (uint8_t)(cpu.regs.A + 0x06);
			}
			if (cpu.regs.A & 0x80) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (cpu.regs.A == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
			break;
		}
	}

	template <int step = 1>
	void DAS(CPU& cpu, bool skipped) {
		switch (step) {
		case 1:
			if (!cpu.get_flag_C() || cpu.regs.A > 0x99) {
				cpu.regs.A = (uint8_t)(cpu.regs.A - 0x60);
				cpu.clear_flag_C();
			}
			break;
		case 2:
			if (!cpu.get_flag_H() || (cpu.regs.A & 0x0F) > 0x09) {
				cpu.regs.A = (uint8_t)(cpu.regs.A - 0x06);
			}
			if (cpu.regs.A & 0x80) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (cpu.regs.A == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
			break;
		}
	}

	template <SubFunc func = SubFunc::None>
	void XCN(CPU& cpu, bool skipped) {
		Byte value = cpu.regs.A;
		value = (value >> 7) | (value << 1);
		cpu.regs.A = (uint8_t)(value);
		SPC700Functions::sub_func(cpu, skipped, func);
	}

	template<int step = 1, SubFunc func = SubFunc::None>
	void DIV(CPU& cpu, bool skipped) {
		switch(step) {
		case 1:
			if ((cpu.regs.Y & 15) >= (cpu.regs.X & 15)) {
				cpu.set_flag_H();
			} else {
				cpu.clear_flag_H();
			}
			if (cpu.regs.Y >= cpu.regs.X) {
				cpu.set_flag_V();
			} else {
				cpu.clear_flag_V();
			}
			cpu.DivYa = (uint32_t)(ya(cpu));
			cpu.ShiftedX = (uint32_t)(get_lo(cpu.regs.X)) << 9;
			break;
		case 2:
			cpu.DivYa = cpu.DivYa << 1;
			if (cpu.DivYa & 0x20000) {
				cpu.DivYa = cpu.DivYa ^ 0x20001;
			}
			if (cpu.DivYa >= cpu.ShiftedX) {
				cpu.DivYa = cpu.DivYa ^ 1;
			}
			if (cpu.DivYa & 1) {
				cpu.DivYa = (cpu.DivYa - cpu.ShiftedX) & 0x1FFFF;
			}
			break;
		case 3:
			cpu.regs.Y = (uint8_t)(cpu.DivYa >> 9);
			cpu.regs.A = (uint8_t)(cpu.DivYa);
			break;
		}
		SPC700Functions::sub_func(cpu, skipped, func);
	}

	// write the value...

	template<int step = 1, SubFunc func = SubFunc::None>
	void MUL(CPU& cpu, bool skipped) {
		switch(step) {
		case 1:
			cpu.YABuffer = cpu.regs.Y * cpu.regs.A;
			break;
		case 2:
			cpu.regs.A = (uint8_t)get_lo(cpu.YABuffer);
			cpu.regs.Y = (uint8_t)get_hi(cpu.YABuffer);
			break;
		}
		SPC700Functions::sub_func(cpu, skipped, func);
	}

	template<int shift = 0>
	void SET(CPU& cpu, bool skipped) {
		Byte mask = (1 << shift);
		cpu.BufferOperand = ((cpu.BufferOperand & ~mask) | mask);
	}

	template<int shift = 0>
	void CLR(CPU& cpu, bool skipped) {
		Byte mask = (1 << shift);
		cpu.BufferOperand = ((cpu.BufferOperand & ~mask) | 0x00);
	}

	template<int shift = 0>
	void BBS(CPU& cpu, bool skipped) {
		cpu.BufferJump = ((cpu.BufferOperand & (1 << shift)) == (1 << shift));
	}

	template<int shift = 0>
	void BBC(CPU& cpu, bool skipped) {
		cpu.BufferJump = ((cpu.BufferOperand & (1 << shift)) == (0 << shift));
	}

	template <int step = 0>
	void Branch(CPU& cpu, bool skipped) {
		switch(step) {
		case 1:
			cpu.BufferAddress = cpu.regs.PC + (int8_t)(cpu.BufferOperand);
			cpu.regs.PC = (get_hi(cpu.regs.PC) << 8) | get_lo(cpu.BufferAddress);
			break;
		case 2:
			cpu.regs.PC = (get_hi(cpu.BufferAddress) << 8) | get_lo(cpu.regs.PC);
			break;
		}
	}

	template <int step = 0>
	void MOV_5D(CPU& cpu, bool skipped) {
		cpu.regs.X = (uint8_t)cpu.regs.A;
	}

	template <int step = 0>
	void MOV_7D(CPU& cpu, bool skipped) {
		cpu.regs.A = (uint8_t)cpu.regs.X;
	}

	template <int step = 0>
	void MOV_8D(CPU& cpu, bool skipped) {
		cpu.regs.Y = (uint8_t)cpu.BufferOperand;
	}

	template <int step = 0>
	void MOV_8F(CPU& cpu, bool skipped) {
		cpu.BufferOperand = (uint8_t)cpu.regs.A;
	}

	template <int step = 0>
	void MOV_9D(CPU& cpu, bool skipped) {
		cpu.regs.X = (uint8_t)cpu.regs.S;
	}

	template <int step = 0>
	void MOV_BD(CPU& cpu, bool skipped) {
		cpu.regs.S = (uint8_t)cpu.regs.X;
	}

	template <int code = 0, int step = 0, SubFunc func = SubFunc::None, bool pc_increment = false>
	void MOV(CPU& cpu, bool skipped) {
		if (pc_increment) { cpu.regs.PC++; }
		switch (code) {
		case 0x5D: SPC700Functions::MOV_5D<step>(cpu, skipped); break;
		case 0x7D: SPC700Functions::MOV_7D<step>(cpu, skipped); break;
		case 0x8D: SPC700Functions::MOV_8D<step>(cpu, skipped); break;
		case 0x8F: SPC700Functions::MOV_8F<step>(cpu, skipped); break;
		case 0x9D: SPC700Functions::MOV_9D<step>(cpu, skipped); break;
		case 0xBD: SPC700Functions::MOV_BD<step>(cpu, skipped); break;
		}

		SPC700Functions::sub_func(cpu, skipped, func);
	}
}

// NOP (00)
Instruction s_00 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// TCALL0 (01)
Instruction s_01 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// SET1 (02)
Instruction s_02 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::SET<0>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBS0 (03)
Instruction s_03 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBS<0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// PUSH (0D)
Instruction s_0d = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::P, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::DecrementS),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BRK (0F)
Instruction s_0f = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::P, WriteTo::StackMinus2>),
	MakeHandler(SPC700Functions::DecrementS<3, SubFunc::ClearISetX>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::FFDE, ReadTo::PCLow>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::FFDF, ReadTo::PCHigh>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// TCALL1 (11)
Instruction s_11 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<1>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<1>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// CLR1 (12)
Instruction s_12 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::CLR<0>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBC0 (13)
Instruction s_13 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBC<0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// CLRP (20)
Instruction s_20 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::CLRP),
	MakeHandler(SPC700Functions::Next)
};

// TCALL2 (21)
Instruction s_21 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// SET2 (22)
Instruction s_22 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::SET<1>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBS1 (23)
Instruction s_23 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBS<1>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// PUSH (2D)
Instruction s_2d = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::A, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::DecrementS),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// TCALL3 (31)
Instruction s_31 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<3>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<3>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// CLR2 (32)
Instruction s_32 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::CLR<1>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBC1 (33)
Instruction s_33 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBC<1>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// SETP (40)
Instruction s_40 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::SETP),
	MakeHandler(SPC700Functions::Next)
};

// TCALL4 (41)
Instruction s_41 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<4>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<4>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// SET3 (42)
Instruction s_42 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::SET<2>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBS2 (43)
Instruction s_43 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// PUSH (4D)
Instruction s_4d = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::X, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::DecrementS),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// TCALL5 (51)
Instruction s_51 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<5>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<5>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// CLR3 (52)
Instruction s_52 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::CLR<2>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBC2 (53)
Instruction s_53 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBC<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// MOV (5D) (Implied)
Instruction s_5d = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::MOV<0x5D, 0, SubFunc::SetNZFlagRegisterX>),
	MakeHandler(SPC700Functions::Next)
};

// CLRC (60)
Instruction s_60 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::CLRC),
	MakeHandler(SPC700Functions::Next)
};

// TCALL6 (61)
Instruction s_61 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<6>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<6>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// SET4 (62)
Instruction s_62 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::SET<3>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBS3 (63)
Instruction s_63 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBS<3>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// PUSH (6D)
Instruction s_6d = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::Y, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::DecrementS),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// RET (6F)
Instruction s_6f = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::IncrementS),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack0, ReadTo::PCLow>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack1, ReadTo::PCHigh>),
	MakeHandler(SPC700Functions::IncrementS),
	MakeHandler(SPC700Functions::Next)
};

// TCALL7 (71)
Instruction s_71 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<7>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<7>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// CLR4 (72)
Instruction s_72 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::CLR<3>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBC3 (73)
Instruction s_73 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBC<3>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// MOV (7D) (Implied)
Instruction s_7d = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::MOV<0x7D, 0, SubFunc::SetNZFlagRegisterA>),
	MakeHandler(SPC700Functions::Next)
};

// RETI (7F)
Instruction s_7f = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::IncrementS),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack0, ReadTo::P>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack1, ReadTo::PCLow>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack2, ReadTo::PCHigh>),
	MakeHandler(SPC700Functions::IncrementS<2>),
	MakeHandler(SPC700Functions::Next)
};

// SETC (80)
Instruction s_80 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::SETC),
	MakeHandler(SPC700Functions::Next)
};

// TCALL8 (81)
Instruction s_81 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<8>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<8>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// SET5 (82)
Instruction s_82 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::SET<4>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBS4 (83)
Instruction s_83 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBS<4>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// MOV (8D) (Immediate)
Instruction s_8d = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::MOV<0x8D, 0, SubFunc::SetNZFlagOperand, true>),
	MakeHandler(SPC700Functions::Next)
};

// POP (8E)
Instruction s_8e = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::IncrementS),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack0, ReadTo::P>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// MOV (8F) (Immediate Data to Direct Page d, #i (Read/Modify/Write))
Instruction s_8f = {
	IMMEDIATE_DATA_TO_DIRECT_PAGE_D_READ_MODIFY_WRITE_START
	MakeHandler(SPC700Functions::MOV<0x8f, 0, SubFunc::None>),
	IMMEDIATE_DATA_TO_DIRECT_PAGE_D_READ_MODIFY_WRITE_END
};

// TCALL9 (91)
Instruction s_91 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<9>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<9>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// CLR5 (92)
Instruction s_92 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::CLR<4>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBC4 (93)
Instruction s_93 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBC<4>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// MOV (9D)
Instruction s_9d = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::MOV<0x9d, 0, SubFunc::SetNZFlagRegisterX>),
	MakeHandler(SPC700Functions::Next)
};

// DIV (9E)
Instruction s_9e = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<1>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DIV<3, SubFunc::SetNZFlagRegisterA>),
	MakeHandler(SPC700Functions::Next),
};

// XCN (9F)
Instruction s_9f = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::XCN),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::XCN),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::XCN),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::XCN<SubFunc::SetNZFlagRegisterA>),
	MakeHandler(SPC700Functions::Next),
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

// TCALL10 (A1)
Instruction s_a1 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<10>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<10>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// SET6 (A2)
Instruction s_a2 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::SET<5>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBS5 (A3)
Instruction s_a3 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBS<5>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// POP (AE)
Instruction s_ae = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::IncrementS),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack0, ReadTo::A>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// MOV (AF)
Instruction s_af = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::SetFuncX),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::A, WriteTo::Address>),
	MakeHandler(SPC700Functions::IncrementX<1>),
	MakeHandler(SPC700Functions::Next)
};

// TCALL11 (B1)
Instruction s_b1 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<11>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<11>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// CLR6 (B2)
Instruction s_b2 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::CLR<5>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBC5 (B3)
Instruction s_b3 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBC<5>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// DAS  (BE)
Instruction s_be = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DAS<1>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DAS<2>),
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

// TCALL12 (C1)
Instruction s_c1 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<12>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<12>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// SET7 (C2)
Instruction s_c2 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::SET<6>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBS6 (C3)
Instruction s_c3 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBS<6>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// POP (CE)
Instruction s_ce = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::IncrementS),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack0, ReadTo::X>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// MUL (CF)
Instruction s_cf = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::MUL<1>),
	MakeHandler(SPC700Functions::MUL<2, SubFunc::SetNZFlagRegisterYA>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// TCALL13 (D1)
Instruction s_d1 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<13>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<13>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// CLR7 (D2)
Instruction s_d2 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::CLR<6>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBC6 (D3)
Instruction s_d3 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBC<6>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

// DAA  (DF)
Instruction s_df = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DAA<1>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::DAA<2>),
	MakeHandler(SPC700Functions::Next)
};

// CLRV (E0)
Instruction s_e0 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::CLRV),
	MakeHandler(SPC700Functions::Next)
};

// TCALL14 (E1)
Instruction s_e1 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<14>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<14>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// SET8 (E2)
Instruction s_e2 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::SET<7>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBS7 (E3)
Instruction s_e3 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBS<7>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
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

// POP (EE)
Instruction s_ee = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Discard>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::IncrementS),
	MakeHandler(SPC700Functions::Read<ReadFrom::Stack0, ReadTo::Y>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// TCALL15 (F1)
Instruction s_f1 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(SPC700Functions::DecrementS<2>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallLow<15>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::TCallHigh<15>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next),	
};

// CLR8 (F2)
Instruction s_f2 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::CLR<7>),
	MakeHandler(SPC700Functions::Write<WriteValue::Operand, WriteTo::Address>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Next)
};

// BBC7 (F3)
Instruction s_f3 = {
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>),
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(SPC700Functions::BBC<7>),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>),
	MakeHandler(SPC700Functions::IncrementPC),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<1>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::NOP, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Branch<2>, SPC700Predicates::NoJump),
	MakeHandler(SPC700Functions::Next)
};

Instruction s_nop = {
	MakeHandler(SPC700Functions::NOP),
	MakeHandler(SPC700Functions::NOP),
};