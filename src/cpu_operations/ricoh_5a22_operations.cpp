#include "ricoh_5a22_operations.hpp"

// Micro-instructions
// Functions are all of the type


#define INSTRUCTION_START_CHECK_ROUTINE ;
#define INSTRUCTION_END_CHECK_ROUTINE ; 
#define PREDICATE_CHECK_ROUTINE ;

#define NEXT_OPCODE MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::OpCode>) 


#define NATIVE_IMMEDIATE_NO_M \
	/* 1.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC), \
	/* 1.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
#define NATIVE_IMMEDIATE_M \
	/* 1.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC), \
	/* 1.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>), \
	/* 2.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::MFlagSet), \
	/* 2.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::OperandHigh>, Ricoh5A22Predicates::MFlagSet),
/*
#define NATIVE_RELATIVE ;
#define NATIVE_PROGRAM_COUNTER_RELATIVE_LONG ;*/

#define NATIVE_DIRECT_READ \
	/* 1.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC), \
	/* 1.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>), \
	/* 2.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero), \
	/* 2.2 */ MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero), \
	/* 3.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, Mode::IfSkipped>), \
	/* 3.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand>), \
	/* 4.1 */ MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet), \
	/* 4.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand, Mode::PlusOne>, Ricoh5A22Predicates::MFlagSet),

/*#define NATIVE_DIRECT_READ_MODIFY_WRITE ;
#define NATIVE_DIRECT_WRITE ;
#define NATIVE_DIRECT_X_READ ;
#define NATIVE_DIRECT_X_READ_MODIFY_WRITE ;
#define NATIVE_DIRECT_X_WRITE ;
#define NATIVE_DIRECT_Y_READ ;
#define NATIVE_DIRECT_Y_WRITE ;
#define NATIVE_DIRECT_PAGE_INDIRECT ;*/

#define NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ \
	/* 1.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC), \
	/* 1.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>), \
	/* 2.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::DX>), \
	/* 2.2 */ MakeHandler(Ricoh5A22Functions::NOP), \
	/* 3.1 */ MakeHandler(Ricoh5A22Functions::Copy<ReadFrom::Pointer, ReadTo::Address, CopyMode::High>, Ricoh5A22Predicates::DLZero), \
	/* 3.2 */ MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero), \
	/* 4.1 */ MakeHandler(Ricoh5A22Functions::NOP), \
	/* 4.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Pointer>), \
	/* 5.1 */ MakeHandler(Ricoh5A22Functions::NOP), \
	/* 5.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Pointer, Mode::PlusOne>), \
	/* 6.1 */ MakeHandler(Ricoh5A22Functions::NOP), \
	/* 6.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerDB, ReadTo::Operand>), \
	/* 7.1 */ MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet), \
	/* 7.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerDB, ReadTo::Operand, Mode::PlusOne>, Ricoh5A22Predicates::MFlagSet),
/*#define NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ_MODIFY_WRITE ;
#define NATIVE_DIRECT_INDEXED_INDIRECT_D_X_WRITE ;
#define NATIVE_DIRECT_INDIRECT_READ ;
#define NATIVE_DIRECT_INDIRECT_WRITE ;
#define NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_READ ;
#define NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_WRITE ;
#define NATIVE_DIRECT_INDIRECT_LONG_READ ;
#define NATIVE_DIRECT_INDIRECT_LONG_WRITE ;
#define NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ ;
#define NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_WRITE ;*/

#define NATIVE_STACK_RELATIVE_READ \
	/* 1.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC), \
	/* 1.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>), \
	/* 2.1 */ MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::APS>), \
	/* 2.2 */ MakeHandler(Ricoh5A22Functions::NOP), \
	/* 3.1 */ MakeHandler(Ricoh5A22Functions::NOP), \
	/* 3.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand>), \
	/* 4.1 */ MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet), \
	/* 4.2 */ MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand, Mode::PlusOne>, Ricoh5A22Predicates::MFlagSet),
/*#define NATIVE_STACK_RELATIVE_WRITE ;
#define NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_READ ;
#define NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_WRITE ;*/

/*
#define NATIVE_ABSOLUTE ;
#define NATIVE_ABSOLUTE_PUSH ;
#define NATIVE_ABSOLUTE_READ ;
#define NATIVE_ABSOLUTE_READ_MODIFY_WRITE ;
#define NATIVE_ABSOLUTE_WRITE ;
#define NATIVE_ABSOLUTE_X_READ ;
#define NATIVE_ABSOLUTE_X_WRITE ;
#define NATIVE_ABSOLUTE_Y_READ ;
#define NATIVE_ABSOLUTE_Y_WRITE ;
#define NATIVE_ABSOLUTE_LONG ;
#define NATIVE_ABSOLUTE_LONG_READ ;
#define NATIVE_ABSOLUTE_LONG_WRITE ;
#define NATIVE_ABSOLUTE_LONG_X_READ ;
#define NATIVE_ABSOLUTE_LONG_X_WRITE ;
#define NATIVE_ABSOLUTE_INDIRECT_JUMP ;
#define NATIVE_ABSOLUTE_INDIRECT_LONG_JUMP ;

#define NATIVE_BLOCK_MOVE_POSITIVE ;
#define NATIVE_BLOCK_MOVE_NEGATIVE ;
*/

namespace ReadTo {
	struct OpCode  {};
	struct Pointer {};
	struct Address {};
	struct Operand {};

	struct OpCodeHigh  {};
	struct PointerHigh {};
	struct AddressHigh {};
	struct OperandHigh {};

	struct PCPB      {};
	struct PointerDB {};
}

namespace ReadFrom {
	struct OpCode  {};
	struct Pointer {};
	struct Address {};
	struct Operand {};

	struct PCPB      {};
	struct PointerDB {};
}

namespace CopyMode {
	struct All  {};
	struct High {};
	struct Low  {};
}

namespace SetMode {
	struct None {};
	struct DX   {};
	struct APS  {};
	struct AOD  {};
}

namespace Mode {
	constexpr bool PlusOne = true;
	constexpr bool PCIncrement = true;
	constexpr bool IfSkipped = true;

	struct Native    {};
	struct Emulation {};
}

Address get_pcpb(Word pc, Byte pb) {
	return (pb << 16) | pc;
}

namespace Ricoh5A22Functions {
	void NOP(CPU& cpu, bool skipped) {
		return;
	}

	template <typename Set = SetMode::None, bool IfSkipped = false>
	void IncrementPC(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (IfSkipped) {
			if (skipped) { cpu.regs.PC++; }
		} else {
			cpu.regs.PC++;
		}

		if constexpr (std::is_same_v<Set, SetMode::DX>) {
			cpu.BufferPointer = cpu.BufferPointer + cpu.regs.X + cpu.regs.D;
			cpu.BufferAddress = cpu.BufferPointer;
		}
		if constexpr (std::is_same_v<Set, SetMode::APS>) {
			cpu.BufferAddress = cpu.BufferPointer + cpu.regs.S;
		}
		if constexpr (std::is_same_v<Set, SetMode::AOD>) {
			cpu.BufferAddress = cpu.BufferOperand + cpu.regs.D;
		}

		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename From, typename To, bool PlusOne = false>
	void Read(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		Word register_offset = 0;
		Byte register_bank = 0;

		if constexpr (std::is_same_v<From, ReadFrom::PCPB>) {
			register_offset = cpu.regs.PC;
			register_bank = cpu.regs.PB;
		}
		if constexpr (std::is_same_v<From, ReadFrom::Address>) {
			register_offset = cpu.BufferAddress;
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::PointerDB>) {
			register_offset = cpu.BufferPointer;
			register_bank = cpu.regs.DB;
		}

		Word* read_to = nullptr;

		if constexpr (std::is_same_v<To, ReadTo::OpCode> || std::is_same_v<To, ReadTo::OpCodeHigh>) {
			read_to = &cpu.BufferOpCode;
		}
		if constexpr (std::is_same_v<To, ReadTo::Pointer> || std::is_same_v<To, ReadTo::PointerHigh>) {
			read_to = &cpu.BufferPointer;
		}
		if constexpr (std::is_same_v<To, ReadTo::Address> || std::is_same_v<To, ReadTo::AddressHigh>) {
			read_to = &cpu.BufferAddress;
		}
		if constexpr (std::is_same_v<To, ReadTo::Operand> || std::is_same_v<To, ReadTo::OperandHigh>) {
			read_to = &cpu.BufferOperand;
		}

		if constexpr (PlusOne) {
			register_offset = register_offset + 1;
		}

		Byte value_read = cpu.read(get_pcpb(register_offset, register_bank));
		Word to_read = value_read;
		if constexpr (PlusOne) {
			to_read = (value_read << 8) | get_lo(*read_to);
		} else {
			to_read = value_read;
		}

		if constexpr(std::is_same_v<To, ReadTo::OpCodeHigh> || std::is_same_v<To, ReadTo::PointerHigh> || std::is_same_v<To, ReadTo::AddressHigh> || std::is_same_v<To, ReadTo::OperandHigh>) {
			*read_to = (to_read << 8) | get_lo(*read_to);
		} else {
			*read_to = to_read;
		}

		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename From, typename To, typename Mode>
	void Copy(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		Word* to = nullptr;
		Word* from = nullptr;

		if constexpr (std::is_same_v<From, ReadFrom::OpCode>) {
			from = &cpu.BufferOpCode;
		}
		if constexpr (std::is_same_v<From, ReadFrom::Pointer>) {
			from = &cpu.BufferPointer;
		}
		if constexpr (std::is_same_v<From, ReadFrom::Address>) {
			from = &cpu.BufferAddress;
		}
		if constexpr (std::is_same_v<From, ReadFrom::Operand>) {
			from = &cpu.BufferOperand;
		}


		if constexpr (std::is_same_v<To, ReadTo::OpCode>) {
			to = &cpu.BufferOpCode;
		}
		if constexpr (std::is_same_v<To, ReadTo::Pointer>) {
			to = &cpu.BufferPointer;
		}
		if constexpr (std::is_same_v<To, ReadTo::Address>) {
			to = &cpu.BufferAddress;
		}
		if constexpr (std::is_same_v<To, ReadTo::Operand>) {
			to = &cpu.BufferOperand;
		}

		if constexpr (std::is_same_v<Mode, CopyMode::All>) {
			*to = *from;
		}
		if constexpr (std::is_same_v<Mode, CopyMode::High>) {
			*to = get_lo(*to);
			*to = (get_hi(*from) << 8) | *to;
		}
		if constexpr (std::is_same_v<Mode, CopyMode::Low>) {
			*to = get_hi(*to);
			*to = ( (*to) << 8) | get_lo(*from);
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}


	template <typename CPUMode, bool PCIncrement = false>
	void LDA(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				cpu.regs.A = (get_hi(cpu.regs.A) << 8) | get_lo(cpu.BufferOperand);
				cpu.set_flag_N(get_lo(cpu.regs.A) & 0x80);
				cpu.set_flag_Z(get_lo(cpu.regs.A));
			} else {
				cpu.regs.A = cpu.BufferOperand;
				cpu.set_flag_N(get_hi(cpu.regs.A) & 0x80);
				cpu.set_flag_Z(cpu.regs.A);
			}
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename CPUMode, bool PCIncrement = false>
	void ORA(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.A = cpu.regs.A | cpu.BufferOperand;
			if (cpu.get_flag_M()) {
				cpu.set_flag_N(get_lo(cpu.regs.A) & 0x80);
				cpu.set_flag_Z(get_lo(cpu.regs.A));
			} else {
				cpu.set_flag_N(get_hi(cpu.regs.A) & 0x80);
				cpu.set_flag_Z(cpu.regs.A);
			}
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename CPUMode, bool PCIncrement = false>
	void AND(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				cpu.regs.A = (get_hi(cpu.regs.A) << 8) | (get_lo(cpu.regs.A) & get_lo(cpu.BufferOperand));
				cpu.set_flag_N(get_lo(cpu.regs.A) & 0x80);
				cpu.set_flag_Z(get_lo(cpu.regs.A));
			} else {
				cpu.regs.A = cpu.regs.A & cpu.BufferOperand;
				cpu.set_flag_N(get_hi(cpu.regs.A) & 0x80);
				cpu.set_flag_Z(cpu.regs.A);
			}
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename CPUMode, bool PCIncrement = false>
	void EOR(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.A = cpu.regs.A ^ cpu.BufferOperand;
			if (cpu.get_flag_M()) {
				cpu.set_flag_N(get_lo(cpu.regs.A) & 0x80);
				cpu.set_flag_Z(get_lo(cpu.regs.A));
			} else {
				cpu.set_flag_N(get_hi(cpu.regs.A) & 0x80);
				cpu.set_flag_Z(cpu.regs.A);
			}
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename CPUMode, bool PCIncrement = false>
	void ADC(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				if (!cpu.get_flag_D()) {
					uint16_t result = get_lo(cpu.regs.A) + cpu.BufferOperand + cpu.get_flag_C();
					if ((~(get_lo(cpu.regs.A) ^ cpu.BufferOperand) & (get_lo(cpu.regs.A) ^ get_lo(result)) & 0x80) != 0) {
						cpu.set_flag_V();
					} else {
						cpu.clear_flag_V();
					}
					if (result > 0xFF) {
						cpu.set_flag_C();
					} else {
						cpu.clear_flag_C();
					}
					result = get_lo(result);
					cpu.regs.A = (get_hi(cpu.regs.A) << 8) | result;
					cpu.set_flag_Z(result);
					cpu.set_flag_N(result & 0x80);
				} else {
					uint16_t lo = (get_lo(cpu.regs.A) & 0x0F) + (cpu.BufferOperand & 0x0F) + cpu.get_flag_C();
					if (lo > 9) {
						lo += 6;
					}
					uint16_t carry_to_hi = (lo > 0x0F) ? 1 : 0;
					uint16_t hi_sum = (get_lo(cpu.regs.A) >> 4) + (cpu.BufferOperand >> 4) + carry_to_hi;
					if ((~((get_lo(cpu.regs.A) >> 4) ^ (cpu.BufferOperand >> 4)) & ((get_lo(cpu.regs.A) >> 4) ^ hi_sum) & 0x08) != 0) {
						cpu.set_flag_V();
					} else {
						cpu.clear_flag_V();
					}
					if (hi_sum > 9) {
						hi_sum += 6;
					}
					if (hi_sum > 0x0f) {
						cpu.set_flag_C();
					} else {
						cpu.clear_flag_C();
					}
					uint16_t result = ((hi_sum & 0x0f) << 4) | (lo & 0x0f);
					cpu.regs.A = (get_hi(cpu.regs.A) << 8) | result;
					cpu.set_flag_Z(result);
					cpu.set_flag_N(result & 0x80);
				}
			} else {
				if (!cpu.get_flag_D()) {
					uint32_t result = cpu.regs.A + cpu.BufferOperand + cpu.get_flag_C();
					if ((~(cpu.regs.A ^ cpu.BufferOperand) & (cpu.regs.A ^ (uint16_t)(result)) & 0x8000) != 0) {
						cpu.set_flag_V();
					} else {
						cpu.clear_flag_V();
					}
					if (result > 0xFFFF) {
						cpu.set_flag_C();
					} else {
						cpu.clear_flag_C();
					}
					result = (uint16_t)(result);
					cpu.regs.A = (uint16_t)result;
					cpu.set_flag_Z(result);
					if ((result & 0x8000) != 0) {
						cpu.set_flag_N();
					} else {
						cpu.clear_flag_N();
					}
				} else {
					uint16_t result = 0;
					uint16_t carry = cpu.get_flag_C();
					for (int i = 0; i < 16; i += 4) {
						uint16_t digit_sum = (uint16_t)( (cpu.regs.A >> i) & 0x0F) + (uint16_t)((cpu.BufferOperand >> i) & 0x0F) + carry;
						if (i == 12) {
							if ((~((cpu.regs.A >> 12) ^ (cpu.BufferOperand >> 12)) & ((cpu.regs.A >> 12) ^ digit_sum) & 0x08) != 0) {
								cpu.set_flag_V();
							} else {
								cpu.clear_flag_V();
							}
						}
						if (digit_sum > 9) {
							digit_sum += 6;
						}
						carry = (digit_sum > 0x0F) ? 1 : 0;
						result = result | (uint16_t)((digit_sum & 0x0F) << i);
					}
					if (carry != 0) {
						cpu.set_flag_C();
					} else {
						cpu.clear_flag_C();
					}
					cpu.regs.A = result;
					cpu.set_flag_Z(result);
					if ((result & 0x8000) != 0) {
						cpu.set_flag_N();
					} else {
						cpu.clear_flag_N();
					}
				}
			}
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}
}

namespace Ricoh5A22Predicates {
	bool Never(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return false;
	}

	bool Even(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return ( (cpu.regs.PC & 0b1) == 0);
	}

	bool Odd(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return ( (cpu.regs.PC & 0b1) == 1);
	}

	bool DLZero(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return get_lo(cpu.regs.D) == 0;
	}

	bool MFlagSet(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return cpu.get_flag_M();
	}
}

// ORA (01)
Instruction n_01 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};

// ORA (09)
Instruction n_09 = {
	NATIVE_IMMEDIATE_M
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::ORA<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};

// AND (21)
Instruction n_21 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};

// AND (29)
Instruction n_29 = {
	NATIVE_IMMEDIATE_M
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::AND<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};

// EOR (41)
Instruction n_41 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};

// EOR (49)
Instruction n_49 = {
	NATIVE_IMMEDIATE_M
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::EOR<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};

// ADC (61)
Instruction n_61 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};

// ADC (69)
Instruction n_69 = {
	NATIVE_IMMEDIATE_M
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::ADC<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};

// LDA (A1)
Instruction n_a1 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};

// LDA (A3)
Instruction n_a3 = {
	NATIVE_STACK_RELATIVE_READ
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};

// LDA (A5)
Instruction n_a5 = {
	NATIVE_DIRECT_READ
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};

// LDA (A7)

// LDA (A9)
Instruction n_a9 = {
	NATIVE_IMMEDIATE_M
	/* 8.1 */ MakeHandler(Ricoh5A22Functions::LDA<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};

// LDA (AD)

// LDA (AF)

// LDA (B1)

// LDA (B2)

// LDA (B3)

// LDA (B5)

// LDA (B7)

// LDA (B9)

// LDA (BD)

// LDA (BF)

Instruction nop = {
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
};