#include "ricoh_5a22_operations.hpp"
#include "ricoh_5a22_addressing_modes.hpp"

// Micro-instructions
// Functions are all of the type

Address get_pcpb(Word pc, Byte pb) {
	return (pb << 16) | pc;
}

namespace ReadTo {
	struct OpCode  {};
	struct Pointer {};
	struct Address {};
	struct Operand {};
	struct Bank    {};

	struct PC {};

	struct Discard {};

	struct OpCodeHigh  {};
	struct PointerHigh {};
	struct AddressHigh {};
	struct OperandHigh {};
	struct BankHigh    {};

	struct OperandLow  {};
	struct AddressLow  {};

	struct PnCPB      {};
	struct PointerDB {};
}

namespace ReadFrom {
	struct OpCode    {};
	struct Pointer   {};
	struct Address   {};
	struct AddressDL {};
	struct Operand   {};
	struct PC {};

	struct RegisterA {};

	struct AddressPlusOne   {};
	struct AddressPlusOneDL {};

	struct AddressPlusTwo   {};
	struct AddressPlusTwoDL {};

	struct PointerPlusOne {};
	
	struct XBank     {};
	struct PointerBank    {};
	struct PointerPlusOneBankCarry {};
	struct PointerPlusOneBankNoCarry {};
	struct PointerDB    {};
	struct PointerPlusOneDBCarry {};
	struct AddressDB               {};
	struct AddressPlusOneDBCarry   {};
	struct AddressBank             {};
	struct AddressPlusOneBankCarry {};
	struct PCPB      {};

	struct Stack0 {};
	struct Stack0Emulation {};
	struct Stack1 {};
	struct Stack2 {};
}

namespace CopyMode {
	struct All  {};
	struct High {};
	struct Low  {};
}

namespace Branching {
	struct None          {};
	struct FlagNative    {};
	struct FlagEmulation {};
}

namespace SetMode {
	// Native
	struct None {};
	struct DX   {};
	struct APS  {};
	struct AOD  {};
	struct AOXD {};
	struct AOYD {};
	struct DBOL {};
	struct OOPC {};
	struct OX   {};
	struct OY   {};
	struct OZ   {};

	// Emulation
	struct DXEmulation   {};
	struct APSEmulation  {};

	struct AOXDEmulation {};
	struct AOYDEmulation {};
}

namespace Mode {
	constexpr bool PlusOne = true;
	constexpr bool PCIncrement = true;
	constexpr bool IfSkipped = true;
	constexpr bool IsImmediate = true;

	struct Native    {};
	struct Emulation {};

	struct Set   {};
	struct Reset {};

	struct RegisterA {};
	struct RegisterX {};
	struct RegisterY {};
	struct Operand   {};

	struct Increase {};
	struct Decrease {};

	struct MFlag {};
	struct XFlag {};
}

namespace BranchMode {
	struct None    {};
	struct N_Zero  {};
	struct N_One   {};
	struct V_Zero  {};
	struct V_One   {};
	struct Always  {};
	struct C_Zero  {};
	struct C_One   {};
	struct Z_Zero  {};
	struct Z_One   {};
}

namespace WriteValue {
	struct OperandLow  {};
	struct OperandHigh {};
	struct RegisterALow {};
	struct RegisterAHigh {};
	struct None        {};
	struct ALow {};
	struct AHigh {};
	struct XLow {};
	struct XHigh {};
	struct YLow {};
	struct YHigh {};
	struct DLow {};
	struct DHigh {};
	struct PCLow {};
	struct PCHigh {};

	struct Zero {};

	struct DB {};
	struct PB {};	
}

namespace WriteTo {
	struct PointerDB        {};
	struct PointerPlusOneDB {};
	struct YDB  {};

	struct Address {};
	struct AddressPlusOne {};

	struct PointerBank {};
	struct PointerPlusOneBankCarry {};

	struct AddressBank {};
	struct AddressPlusOneBankCarry {};

	struct AddressDB {};
	struct AddressPlusOneDBCarry {};
	struct None {};

	struct Stack0 {};
	struct Stack0Emulation {};
	struct StackMinus1 {};
	struct StackMinus1Emulation {};
	struct StackMinus2 {};
	struct StackMinus2Emulation {};	
}

// Operations which will require additional features when added
namespace Ricoh5A22SpecialFunctions {
	void STOP(CPU& cpu, bool skipped) {
		// STOP BEHAVIOUR TO OCCUR HERE
		// Stops processor until hardware reset
		return;
	}

	void WAIT(CPU& cpu, bool skipped) {
		// WAIT BEHAVIOUR TO OCCUR HERE
		// Places hardware in waiting state (cycle continues forever, until interrupt - I think)?
		return;
	}
}

namespace Ricoh5A22Functions {
	void NOP(CPU& cpu, bool skipped) {
		return;
	}

	void DecrementPC(CPU& cpu, bool skipped) {
		cpu.regs.PC -= 1;
	}

	template <typename Set = SetMode::None, bool IfSkipped = false, typename Branch = BranchMode::None, bool NoIncrement = false>
	void IncrementPC(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (IfSkipped) {
			if (skipped && !NoIncrement) { cpu.regs.PC++; }
		} else {
			if constexpr (!NoIncrement) {
				cpu.regs.PC++;
			}
		}

		if constexpr (std::is_same_v<Branch, BranchMode::N_One>) {
			cpu.Branching = (cpu.get_flag_N());
		}
		if constexpr (std::is_same_v<Branch, BranchMode::N_Zero>) {
			cpu.Branching = !(cpu.get_flag_N());
		}
		if constexpr (std::is_same_v<Branch, BranchMode::V_One>) {
			cpu.Branching = (cpu.get_flag_V());
		}
		if constexpr (std::is_same_v<Branch, BranchMode::V_Zero>) {
			cpu.Branching = !(cpu.get_flag_V());
		}
		if constexpr (std::is_same_v<Branch, BranchMode::C_One>) {
			cpu.Branching = (cpu.get_flag_C());
		}
		if constexpr (std::is_same_v<Branch, BranchMode::C_Zero>) {
			cpu.Branching = !(cpu.get_flag_C());
		}
		if constexpr (std::is_same_v<Branch, BranchMode::Z_One>) {
			cpu.Branching = (cpu.get_flag_Z());
		}
		if constexpr (std::is_same_v<Branch, BranchMode::Z_Zero>) {
			cpu.Branching = !(cpu.get_flag_Z());
		}
		if constexpr (std::is_same_v<Branch, BranchMode::Always>) {
			cpu.Branching = true;
		}

		if constexpr (std::is_same_v<Set, SetMode::DX>) {
			cpu.BufferPointer = cpu.BufferPointer + cpu.regs.X + cpu.regs.D;
			cpu.BufferAddress = cpu.BufferPointer;
		}
		if constexpr (std::is_same_v<Set, SetMode::DXEmulation>) {
			cpu.BufferPointer = cpu.BufferPointer + cpu.regs.X + cpu.regs.D;
			cpu.BufferAddress = (get_hi(cpu.regs.D) << 8) | get_lo(cpu.BufferPointer);
		}
		if constexpr (std::is_same_v<Set, SetMode::APS>) {
			cpu.BufferAddress = cpu.BufferPointer + cpu.regs.S;
		}
		if constexpr (std::is_same_v<Set, SetMode::APSEmulation>) {
			cpu.BufferAddress = cpu.BufferPointer + (cpu.regs.S | 0x0100);
		}
		if constexpr (std::is_same_v<Set, SetMode::AOD>) {
			cpu.BufferAddress = cpu.BufferOperand + cpu.regs.D;
		}
		if constexpr (std::is_same_v<Set, SetMode::AOXD>) {
			cpu.BufferAddress = cpu.BufferOperand + cpu.regs.X + cpu.regs.D;
		}
		if constexpr (std::is_same_v<Set, SetMode::AOXDEmulation>) {
			if (get_lo(cpu.regs.D) == 0) {
				cpu.BufferAddress = (cpu.regs.D & 0xFF00) | (uint8_t)(uint8_t(cpu.BufferOperand) + uint8_t(cpu.regs.X) + uint8_t(cpu.regs.D));
			} else {
				cpu.BufferAddress = cpu.BufferOperand + get_lo(cpu.regs.X) + cpu.regs.D;
			}
		}
		if constexpr (std::is_same_v<Set, SetMode::AOYD>) {
			cpu.BufferAddress = cpu.BufferOperand + cpu.regs.Y + cpu.regs.D;
		}
		if constexpr (std::is_same_v<Set, SetMode::AOYDEmulation>) {
			if (get_lo(cpu.regs.D) == 0) {
				cpu.BufferAddress = (cpu.regs.D & 0xFF00) | (uint8_t)(uint8_t(cpu.BufferOperand) + uint8_t(cpu.regs.Y) + uint8_t(cpu.regs.D));
			} else {
				cpu.BufferAddress = cpu.BufferOperand + get_lo(cpu.regs.Y) + cpu.regs.D;
			}
		}
		if constexpr (std::is_same_v<Set, SetMode::DBOL>) {
			cpu.regs.DB = get_lo(cpu.BufferOperand);
		}
		if constexpr (std::is_same_v<Set, SetMode::OOPC>) {
			cpu.BufferOperand = cpu.BufferOperand + cpu.regs.PC;
		}
		if constexpr (std::is_same_v<Set, SetMode::OX>) {
			cpu.BufferOperand = cpu.regs.X;
		}
		if constexpr (std::is_same_v<Set, SetMode::OY>) {
			cpu.BufferOperand = cpu.regs.Y;
		}
		if constexpr (std::is_same_v<Set, SetMode::OZ>) {
			cpu.BufferOperand = 0;
		}

		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename From, typename To, bool PlusOne = false, typename BranchingRoutine = Branching::None>
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
		if constexpr (std::is_same_v<From, ReadFrom::Pointer>) {
			register_offset = cpu.BufferPointer;
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::PointerPlusOne>) {
			register_offset = cpu.BufferPointer + 1;
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::AddressDL>) {
			if (get_lo(cpu.regs.D) != 0) {
				register_offset = cpu.BufferAddress;
			} else {
				register_offset = (cpu.BufferAddress & 0xFF00) | (uint8_t)(get_lo(cpu.BufferAddress));
			}
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::AddressPlusOneDL>) {
			if (get_lo(cpu.regs.D) != 0) {
				register_offset = cpu.BufferAddress + 1;
			} else {
				register_offset = (cpu.BufferAddress & 0xFF00) | (uint8_t)(get_lo(cpu.BufferAddress + 1));
			}
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::AddressPlusTwoDL>) {
			if (get_lo(cpu.regs.D) != 0) {
				register_offset = cpu.BufferAddress + 2;
			} else {
				register_offset = (cpu.BufferAddress & 0xFF00) | (uint8_t)(get_lo(cpu.BufferAddress + 2));
			}
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::AddressPlusTwo>) {
			register_offset = cpu.BufferAddress + 2;
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::PointerDB>) {
			register_offset = cpu.BufferPointer;
			register_bank = cpu.regs.DB;
		}
		if constexpr (std::is_same_v<From, ReadFrom::PointerBank>) {
			register_offset = cpu.BufferPointer;
			register_bank = cpu.BufferBank;
		}
		if constexpr (std::is_same_v<From, ReadFrom::XBank>) {
			register_offset = cpu.regs.X;
			register_bank = cpu.BufferBank;
		}
		if constexpr (std::is_same_v<From, ReadFrom::PointerPlusOneBankCarry>) {
			register_offset = cpu.BufferPointer + 1;
			register_bank = cpu.BufferBank;

			if (cpu.BufferPointer == 0xFFFF) {
				register_bank++;
			}
		}
		if constexpr (std::is_same_v<From, ReadFrom::PointerPlusOneBankNoCarry>) {
			register_offset = cpu.BufferPointer + 1;
			register_bank = cpu.BufferBank;
		}
		if constexpr (std::is_same_v<From, ReadFrom::PointerDB>) {
			register_offset = cpu.BufferPointer;
			register_bank = cpu.regs.DB;
		}
		if constexpr (std::is_same_v<From, ReadFrom::PointerPlusOneDBCarry>) {
			register_offset = cpu.BufferPointer + 1;
			register_bank = cpu.regs.DB;

			if (cpu.BufferPointer == 0xFFFF) {
				register_bank++;
			}
		}
		if constexpr (std::is_same_v<From, ReadFrom::AddressDB>) {
			register_offset = cpu.BufferAddress;
			register_bank = cpu.regs.DB;
		}
		if constexpr (std::is_same_v<From, ReadFrom::AddressPlusOneDBCarry>) {
			register_offset = cpu.BufferAddress + 1;
			register_bank = cpu.regs.DB;

			if (cpu.BufferAddress == 0xFFFF) {
				register_bank++;
			}
		}
		if constexpr (std::is_same_v<From, ReadFrom::AddressBank>) {
			register_offset = cpu.BufferAddress;
			register_bank = cpu.BufferBank;
		}
		if constexpr (std::is_same_v<From, ReadFrom::AddressPlusOneBankCarry>) {
			register_offset = cpu.BufferAddress + 1;
			register_bank = cpu.BufferBank;

			if (cpu.BufferAddress == 0xFFFF) {
				register_bank++;
			}
		}
		if constexpr (std::is_same_v<From, ReadFrom::Stack0>) {
			register_offset = cpu.regs.S;
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::Stack1>) {
			register_offset = cpu.regs.S + 1;
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::Stack2>) {
			register_offset = cpu.regs.S + 2;
			register_bank = 0;
		}
		if constexpr (std::is_same_v<From, ReadFrom::Stack0Emulation>) {
			register_offset = 0x0100 | (uint8_t)(get_lo(cpu.regs.S));
			register_bank = 0;
		}


		Word* read_to = nullptr;

		if constexpr (std::is_same_v<To, ReadTo::OpCode> || std::is_same_v<To, ReadTo::OpCodeHigh>) {
			read_to = &cpu.BufferOpCode;
		}
		if constexpr (std::is_same_v<To, ReadTo::Pointer> || std::is_same_v<To, ReadTo::PointerHigh>) {
			read_to = &cpu.BufferPointer;
		}
		if constexpr (std::is_same_v<To, ReadTo::Address> || std::is_same_v<To, ReadTo::AddressHigh> || std::is_same_v<To, ReadTo::AddressLow>) {
			read_to = &cpu.BufferAddress;
		}
		if constexpr (std::is_same_v<To, ReadTo::Operand> || std::is_same_v<To, ReadTo::OperandHigh> || std::is_same_v<To, ReadTo::OperandLow>) {
			read_to = &cpu.BufferOperand;
		}
		if constexpr (std::is_same_v<To, ReadTo::Bank> || std::is_same_v<To, ReadTo::BankHigh>) {
			read_to = &cpu.BufferBank;
		}
		if constexpr (std::is_same_v<To, ReadTo::Discard>) {
			read_to = &cpu.DiscardBuffer;
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
		} else if constexpr(std::is_same_v<To, ReadTo::OperandLow> || std::is_same_v<To, ReadTo::AddressLow>) {
			*read_to = (get_hi(*read_to) << 8) | (uint8_t)(to_read);
		} else {
			*read_to = to_read;
		}

		if constexpr(std::is_same_v<To, ReadTo::Bank>) {
			cpu.BufferMVDest = to_read;
		}

		if constexpr (std::is_same_v<BranchingRoutine, Branching::FlagEmulation>) {
			cpu.BufferAddress = (int16_t)((int8_t)(get_lo(cpu.BufferOperand))) + cpu.regs.PC + 1;
			cpu.BoundaryCrossed = (get_hi(cpu.BufferAddress) != get_hi(cpu.regs.PC + 1));
			if (!cpu.Branching || !cpu.BoundaryCrossed) {
				cpu.poll_interrupts();
			}
		}
		if constexpr (std::is_same_v<BranchingRoutine, Branching::FlagNative>) {
			cpu.BufferAddress = (int16_t)((int8_t)(get_lo(cpu.BufferOperand))) + cpu.regs.PC + 1;
			if (!cpu.Branching) {
				cpu.poll_interrupts();
			}
		}

		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename From, typename To, typename Mode = CopyMode::All, bool PCIncrement = false>
	void Copy(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		Word* to = nullptr;
		Word* from = nullptr;

		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}

		if constexpr (std::is_same_v<From, ReadFrom::OpCode>) {
			from = &cpu.BufferOpCode;
		}
		if constexpr (std::is_same_v<From, ReadFrom::RegisterA>) {
			from = &cpu.regs.A;
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
		if constexpr (std::is_same_v<To, ReadFrom::PC>) {
			from = &cpu.regs.PC;
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
		if constexpr (std::is_same_v<To, ReadTo::PC>) {
			to = &cpu.regs.PC;
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
		} else {
			cpu.regs.A = (get_hi(cpu.regs.A) << 8) | get_lo(cpu.BufferOperand);
			cpu.set_flag_N(get_lo(cpu.regs.A) & 0x80);
			cpu.set_flag_Z(get_lo(cpu.regs.A));
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename CPUMode, bool PCIncrement = false>
	void LDX(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_X()) {
				cpu.regs.X = (get_hi(cpu.regs.X) << 8) | get_lo(cpu.BufferOperand);
				cpu.set_flag_N(get_lo(cpu.regs.X) & 0x80);
				cpu.set_flag_Z(get_lo(cpu.regs.X));
			} else {
				cpu.regs.X = cpu.BufferOperand;
				cpu.set_flag_N(get_hi(cpu.regs.X) & 0x80);
				cpu.set_flag_Z(cpu.regs.X);
			}
		} else {
			cpu.regs.X = (get_hi(cpu.regs.X) << 8) | get_lo(cpu.BufferOperand);
			cpu.set_flag_N(get_lo(cpu.regs.X) & 0x80);
			cpu.set_flag_Z(get_lo(cpu.regs.X));
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	void BRL(CPU& cpu, bool skipped) {
		cpu.regs.PC += cpu.BufferOperand;
	}

	void SEC(CPU& cpu, bool skipped) {
		cpu.set_flag_C();
	}

	void SEI(CPU& cpu, bool skipped) {
		cpu.set_flag_I();
	}

	void SED(CPU& cpu, bool skipped) {
		cpu.set_flag_D();
	}

	void CLC(CPU& cpu, bool skipped) {
		cpu.clear_flag_C();
	}

	void CLD(CPU& cpu, bool skipped) {
		cpu.clear_flag_D();
	}

	void CLI(CPU& cpu, bool skipped) {
		cpu.clear_flag_I();
	}

	void CLV(CPU& cpu, bool skipped) {
		cpu.clear_flag_V();
	}

	void XCE(CPU& cpu, bool skipped) {
		bool carry = cpu.get_flag_C();
		bool emulation = cpu.regs.emulation_mode;

		if (emulation) {
			cpu.set_flag_C();
		} else {
			cpu.clear_flag_C();
		}

		cpu.regs.emulation_mode = carry;

		if (cpu.regs.emulation_mode) {
			cpu.set_flag_M();
			cpu.set_flag_X();
			cpu.regs.S = cpu.regs.S | 0xFF00;
			cpu.regs.X = cpu.regs.X & 0x00FF;
			cpu.regs.Y = cpu.regs.Y & 0x00FF;
		}
	}

	template <typename CPUMode, bool PCIncrement = false>
	void LDY(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_X()) {
				cpu.regs.Y = (get_hi(cpu.regs.Y) << 8) | get_lo(cpu.BufferOperand);
				cpu.set_flag_N(get_lo(cpu.regs.Y) & 0x80);
				cpu.set_flag_Z(get_lo(cpu.regs.Y));
			} else {
				cpu.regs.Y = cpu.BufferOperand;
				cpu.set_flag_N(get_hi(cpu.regs.Y) & 0x80);
				cpu.set_flag_Z(cpu.regs.Y);
			}
		} else {
			cpu.regs.Y = (get_hi(cpu.regs.Y) << 8) | get_lo(cpu.BufferOperand);
			cpu.set_flag_N(get_lo(cpu.regs.Y) & 0x80);
			cpu.set_flag_Z(get_lo(cpu.regs.Y));
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
		} else {
			cpu.regs.A = cpu.regs.A | cpu.BufferOperand;
			cpu.set_flag_N(get_lo(cpu.regs.A) & 0x80);
			cpu.set_flag_Z(get_lo(cpu.regs.A));
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	template <typename CPUMode, bool PCIncrement = false>
	void CMP(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				cpu.clear_flag_C();
				cpu.clear_flag_Z();
				cpu.clear_flag_N();
				if (get_lo(cpu.regs.A) >= get_lo(cpu.BufferOperand)) {
					cpu.set_flag_C();
				}
				if (get_lo(cpu.regs.A) == get_lo(cpu.BufferOperand)) {
					cpu.set_flag_Z();
				}
				if (((get_lo(cpu.regs.A) - get_lo(cpu.BufferOperand)) & 0x80) != 0) {
					cpu.set_flag_N();
				}
			} else {
				cpu.clear_flag_C();
				cpu.clear_flag_Z();
				cpu.clear_flag_N();
				if (cpu.regs.A >= cpu.BufferOperand) {
					cpu.set_flag_C();
				}
				if (cpu.regs.A == cpu.BufferOperand) {
					cpu.set_flag_Z();
				}
				if (((cpu.regs.A - cpu.BufferOperand) & 0x8000) != 0) {
					cpu.set_flag_N();
				}
			}
		} else {
			cpu.clear_flag_C();
			cpu.clear_flag_Z();
			cpu.clear_flag_N();
			if (get_lo(cpu.regs.A) >= get_lo(cpu.BufferOperand)) {
				cpu.set_flag_C();
			}
			if (get_lo(cpu.regs.A) == get_lo(cpu.BufferOperand)) {
				cpu.set_flag_Z();
			}
			if (((get_lo(cpu.regs.A) - get_lo(cpu.BufferOperand)) & 0x80) != 0) {
				cpu.set_flag_N();
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
		} else {
			cpu.regs.A = (get_hi(cpu.regs.A) << 8) | (get_lo(cpu.regs.A) & get_lo(cpu.BufferOperand));
			cpu.set_flag_N(get_lo(cpu.regs.A) & 0x80);
			cpu.set_flag_Z(get_lo(cpu.regs.A));
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
		} else {
			cpu.regs.A = cpu.regs.A ^ cpu.BufferOperand;
			cpu.set_flag_N(get_lo(cpu.regs.A) & 0x80);
			cpu.set_flag_Z(get_lo(cpu.regs.A));
		}

		INSTRUCTION_END_CHECK_ROUTINE
	}

	void adc_m_flag(CPU& cpu) {
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
	}

	void adc_no_m_flag(CPU& cpu) {
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

	template <typename CPUMode, bool PCIncrement = false>
	void ADC(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				Ricoh5A22Functions::adc_m_flag(cpu);
			} else {
				Ricoh5A22Functions::adc_no_m_flag(cpu);
			}
		} else {
			Ricoh5A22Functions::adc_m_flag(cpu);
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	void sbc_m_flag(CPU& cpu) {
		if (!cpu.get_flag_D()) {
			uint16_t result = get_lo(cpu.regs.A) - cpu.BufferOperand - (1 - cpu.get_flag_C());

			if (((get_lo(cpu.regs.A) ^ cpu.BufferOperand) & (get_lo(cpu.regs.A) ^ get_lo(result)) & 0x80) != 0) {
				cpu.set_flag_V();
			} else {
				cpu.clear_flag_V();
			}

			if (result <= 0xFF) {
				cpu.set_flag_C();
			} else {
				cpu.clear_flag_C();
			}

			result = get_lo(result);
			cpu.regs.A = (get_hi(cpu.regs.A) << 8) | result;
			cpu.set_flag_Z(result);
			cpu.set_flag_N(result & 0x80);
		} else {
			int16_t lo = (get_lo(cpu.regs.A) & 0x0F) - (cpu.BufferOperand & 0x0F) - (1 - cpu.get_flag_C());

			if (lo < 0) {
				lo -= 6;
			}

			int16_t borrow_to_hi = (lo < 0) ? 1 : 0;

			int16_t hi_sum = (get_lo(cpu.regs.A) >> 4) - (cpu.BufferOperand >> 4) - borrow_to_hi;

			if ((((get_lo(cpu.regs.A) >> 4) ^ (cpu.BufferOperand >> 4)) & ((get_lo(cpu.regs.A) >> 4) ^ hi_sum) & 0x08) != 0) {
				cpu.set_flag_V();
			} else {
				cpu.clear_flag_V();
			}

			if (hi_sum < 0) {
				hi_sum -= 6;
			}

			if (hi_sum >= 0) {
				cpu.set_flag_C();
			} else {
				cpu.clear_flag_C();
			}

			uint16_t result = ((hi_sum & 0x0F) << 4) | (lo & 0x0F);
			cpu.regs.A = (get_hi(cpu.regs.A) << 8) | result;
			cpu.set_flag_Z(result);
			cpu.set_flag_N(result & 0x80);
		}
	}

	void sbc_no_m_flag(CPU& cpu) {
		if (!cpu.get_flag_D()) {
			uint32_t result = (uint32_t)cpu.regs.A - (uint32_t)cpu.BufferOperand - (1 - cpu.get_flag_C());

			if (((cpu.regs.A ^ cpu.BufferOperand) & (cpu.regs.A ^ (uint16_t)result) & 0x8000) != 0) {
				cpu.set_flag_V();
			} else {
				cpu.clear_flag_V();
			}

			if (result <= 0xFFFF) {
				cpu.set_flag_C();
			} else {
				cpu.clear_flag_C();
			}

			result = (uint16_t)result;
			cpu.regs.A = (uint16_t)result;
			cpu.set_flag_Z(result);

			if ((result & 0x8000) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
		} else {
			uint16_t result = 0;
			uint16_t borrow = 1 - cpu.get_flag_C();

			for (int i = 0; i < 16; i += 4) {
				int16_t digit_sum = (int16_t)((cpu.regs.A >> i) & 0x0F) - (int16_t)((cpu.BufferOperand >> i) & 0x0F) - borrow;

				if (i == 12) {
					if ((((cpu.regs.A >> 12) ^ (cpu.BufferOperand >> 12)) & ((cpu.regs.A >> 12) ^ digit_sum) & 0x08) != 0) {
						cpu.set_flag_V();
					} else {
						cpu.clear_flag_V();
					}
				}

				if (digit_sum < 0) {
					digit_sum -= 6;
				}

				borrow = (digit_sum < 0) ? 1 : 0;

				result = result | ((uint16_t)((digit_sum & 0x0F) << i));
			}

			if (borrow == 0) {
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

	template <typename CPUMode, bool PCIncrement = false>
	void SBC(CPU& cpu, bool skipped) {
		INSTRUCTION_START_CHECK_ROUTINE
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				Ricoh5A22Functions::sbc_m_flag(cpu);
			} else {
				Ricoh5A22Functions::sbc_no_m_flag(cpu);
			}
		} else {
			Ricoh5A22Functions::sbc_m_flag(cpu);
		}
		INSTRUCTION_END_CHECK_ROUTINE
	}

	template<bool PCIncrement = false>
	void DirectIndirectYIndex(CPU& cpu, bool skipped) {
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}

		cpu.BufferOrig = cpu.BufferPointer;
		uint32_t tmp = (uint32_t)(cpu.BufferPointer + cpu.regs.Y);
		cpu.BufferPointer = (uint16_t)(tmp);
		cpu.BufferBank = cpu.regs.DB + (tmp >> 16);
	}

	template<bool PCIncrement = false>
	void AbsoluteYIndex(CPU& cpu, bool skipped) {
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}

		cpu.BufferOrig = cpu.BufferPointer;
		uint32_t tmp = (uint32_t)(cpu.BufferPointer + cpu.regs.Y);
		cpu.BufferPointer = (uint16_t)(tmp);
		cpu.BufferBank = cpu.regs.DB + (tmp >> 16);
	}

	template<bool PCIncrement = false>
	void AbsoluteXIndex(CPU& cpu, bool skipped) {
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}

		cpu.BufferOrig = cpu.BufferPointer;
		uint32_t tmp = (uint32_t)(cpu.BufferPointer + cpu.regs.X);
		cpu.BufferPointer = (uint16_t)(tmp);
		cpu.BufferBank = cpu.regs.DB + (tmp >> 16);
	}

	template<bool PCIncrement = false>
	void AbsoluteXRMWIndex(CPU& cpu, bool skipped) {
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}

		uint32_t tmp = (uint32_t)(cpu.BufferPointer + cpu.regs.X);
		cpu.BufferPointer = (uint16_t)(tmp);
		cpu.BufferBank = cpu.regs.DB + (tmp >> 16);
	}

	template<bool PCIncrement = false>
	void AbsoluteLongXIndex(CPU& cpu, bool skipped) {
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}

		uint32_t tmp = (uint32_t)(cpu.BufferPointer + cpu.regs.X);
		cpu.BufferPointer = (uint16_t)(tmp);
		cpu.BufferBank += (tmp >> 16);
	}

	template <bool PCIncrement = false>
	void DirectIndirectIndexedLongYIndex(CPU& cpu, bool skipped) {
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}

		uint32_t tmp = (uint32_t)(cpu.BufferPointer + cpu.regs.Y);
		cpu.BufferPointer = (uint16_t)(tmp);
		cpu.BufferBank += (tmp >> 16);
	}

	template<bool PCIncrement = false>
	void StackRelativeIndirectIndexed(CPU& cpu, bool skipped) {
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}

		uint32_t tmp = (uint32_t)(cpu.BufferPointer + cpu.regs.Y);
		cpu.BufferPointer = (uint16_t)tmp;
		cpu.BufferBank = cpu.regs.DB + (tmp >> 16);
	}

	void PollInterrupts(CPU& cpu, bool skipped) {
		cpu.poll_interrupts();
	}

	template<typename Value, typename To>
	void Write(CPU& cpu, bool skipped) {
		Byte value;
		if constexpr (std::is_same_v<Value, WriteValue::OperandLow>) {
			value = (uint8_t)(get_lo(cpu.BufferOperand));
		}
		if constexpr (std::is_same_v<Value, WriteValue::OperandHigh>) {
			value = (uint8_t)(get_hi(cpu.BufferOperand));
		}
		if constexpr (std::is_same_v<Value, WriteValue::PCLow>) {
			value = (uint8_t)(get_lo(cpu.regs.PC));
		}
		if constexpr (std::is_same_v<Value, WriteValue::PCHigh>) {
			value = (uint8_t)(get_hi(cpu.regs.PC));
		}
		if constexpr (std::is_same_v<Value, WriteValue::ALow>) {
			value = (uint8_t)(get_lo(cpu.regs.A));
		}
		if constexpr (std::is_same_v<Value, WriteValue::AHigh>) {
			value = (uint8_t)(get_hi(cpu.regs.A));
		}
		if constexpr (std::is_same_v<Value, WriteValue::XLow>) {
			value = (uint8_t)(get_lo(cpu.regs.X));
		}
		if constexpr (std::is_same_v<Value, WriteValue::XHigh>) {
			value = (uint8_t)(get_hi(cpu.regs.X));
		}
		if constexpr (std::is_same_v<Value, WriteValue::YLow>) {
			value = (uint8_t)(get_lo(cpu.regs.Y));
		}
		if constexpr (std::is_same_v<Value, WriteValue::YHigh>) {
			value = (uint8_t)(get_hi(cpu.regs.Y));
		}
		if constexpr (std::is_same_v<Value, WriteValue::DLow>) {
			value = (uint8_t)(get_lo(cpu.regs.D));
		}
		if constexpr (std::is_same_v<Value, WriteValue::DHigh>) {
			value = (uint8_t)(get_hi(cpu.regs.D));
		}
		if constexpr (std::is_same_v<Value, WriteValue::RegisterALow>) {
			value = (uint8_t)(get_lo(cpu.regs.A));
		}
		if constexpr (std::is_same_v<Value, WriteValue::RegisterAHigh>) {
			value = (uint8_t)(get_hi(cpu.regs.A));
		}
		if constexpr (std::is_same_v<Value, WriteValue::DB>) {
			value = cpu.regs.DB;
		}
		if constexpr (std::is_same_v<Value, WriteValue::PB>) {
			value = cpu.regs.PB;
		}
		if constexpr (std::is_same_v<Value, WriteValue::Zero>) {
			value = 0;
		}

		Address address;
		if constexpr (std::is_same_v<To, WriteTo::YDB>) {
			address = (cpu.regs.DB << 16) | ((uint16_t)(cpu.regs.Y));
		}
		if constexpr (std::is_same_v<To, WriteTo::PointerDB>) {
			address = (cpu.regs.DB << 16) | ((uint16_t)(cpu.BufferPointer));
		}
		if constexpr (std::is_same_v<To, WriteTo::PointerPlusOneDB>) {
			address = (cpu.regs.DB << 16) | ((uint16_t)(cpu.BufferPointer + 1));
		}

		if constexpr (std::is_same_v<To, WriteTo::Address>) {
			address = cpu.BufferAddress;
		}
		if constexpr (std::is_same_v<To, WriteTo::AddressPlusOne>) {
			uint16_t offset = (uint16_t)(cpu.BufferAddress) + 1;
			address = offset;
		}

		if constexpr (std::is_same_v<To, WriteTo::PointerBank>) {
			address = (cpu.BufferBank << 16) | (uint16_t)(cpu.BufferPointer);
		}

		if constexpr (std::is_same_v<To, WriteTo::AddressDB>) {
			address = (cpu.regs.DB << 16) | (uint16_t)(cpu.BufferAddress);
		}

		if constexpr (std::is_same_v<To, WriteTo::AddressBank>) {
			address = (cpu.BufferBank << 16) | (uint16_t)(cpu.BufferAddress);
		}

		if constexpr (std::is_same_v<To, WriteTo::PointerPlusOneBankCarry>) {
			uint16_t register_offset = cpu.BufferPointer + 1;
			uint8_t register_bank = cpu.BufferBank;

			if (cpu.BufferPointer == 0xFFFF) {
				register_bank++;
			}

			address = (register_bank << 16) | register_offset;
		}

		if constexpr (std::is_same_v<To, WriteTo::AddressPlusOneDBCarry>) {
			uint16_t register_offset = cpu.BufferAddress + 1;
			uint8_t register_bank = cpu.regs.DB;

			if (cpu.BufferAddress == 0xFFFF) {
				register_bank++;
			}

			address = (register_bank << 16) | register_offset;
		}

		if constexpr (std::is_same_v<To, WriteTo::AddressPlusOneBankCarry>) {
			uint16_t register_offset = cpu.BufferAddress + 1;
			uint8_t register_bank = cpu.BufferBank;

			if (cpu.BufferAddress == 0xFFFF) {
				register_bank++;
			}

			address = (register_bank << 16) | register_offset;
		}

		if constexpr (std::is_same_v<To, WriteTo::Stack0>) {
			address = cpu.regs.S;
		}
		if constexpr (std::is_same_v<To, WriteTo::Stack0Emulation>) {
			address = 0x0100 | get_lo(cpu.regs.S);
		}
		if constexpr (std::is_same_v<To, WriteTo::StackMinus1>) {
			address = cpu.regs.S - 1;
		}
		if constexpr (std::is_same_v<To, WriteTo::StackMinus1Emulation>) {
			address = (0x0100 | (uint8_t)(get_lo(cpu.regs.S) - 1));
		}
		if constexpr (std::is_same_v<To, WriteTo::StackMinus2>) {
			address = cpu.regs.S - 2;
		}
		if constexpr (std::is_same_v<To, WriteTo::StackMinus2Emulation>) {
			address = (0x0100 | (uint8_t)(get_lo(cpu.regs.S) - 2));
		}

		if constexpr (SST_TEST) {
			cpu.test_poke(address, value);
		} else {
			cpu.write(address, value);
		}
	}

	void MVP(CPU& cpu, bool skipped) {
	    cpu.regs.A -= 1;
	    
	    if (cpu.get_flag_X()) {
	        Byte low = get_lo(cpu.regs.X);
	        low -= 1;
	        cpu.regs.X = (uint8_t)low;
	        
	        low = get_lo(cpu.regs.Y);
	        low -= 1;
	        cpu.regs.Y = (uint8_t)low;
	    } else {
	        cpu.regs.X -= 1;
	        cpu.regs.Y -= 1;
	    }

	    if (cpu.regs.A != 0xFFFF) {
	        cpu.regs.PC -= 3;
	    }
	}

	void MVN(CPU& cpu, bool skipped) {
	    cpu.regs.A -= 1;
	    
	    if (cpu.get_flag_X()) {
	        Byte low = get_lo(cpu.regs.X);
	        low += 1;
	        cpu.regs.X = (uint8_t)low;
	        
	        low = get_lo(cpu.regs.Y);
	        low += 1;
	        cpu.regs.Y = (uint8_t)low;
	    } else {
	        cpu.regs.X += 1;
	        cpu.regs.Y += 1;
	    }

	    if (cpu.regs.A != 0xFFFF) {
	        cpu.regs.PC -= 3;
	    }
	}

	template <typename CPUMode, typename OpMode>
	void TB(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				if ( (get_lo(cpu.regs.A) & get_lo(cpu.BufferOperand)) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				if ( (cpu.regs.A & cpu.BufferOperand) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			if ( (get_lo(cpu.regs.A) & get_lo(cpu.BufferOperand)) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
		if constexpr (std::is_same_v<OpMode, Mode::Set>) {
			cpu.BufferOperand = cpu.BufferOperand | cpu.regs.A;
		} else {
			cpu.BufferOperand = cpu.BufferOperand & ~cpu.regs.A;
		}
	}

	template <typename CPUMode, typename SetMode = Mode::Operand>
	void ASL(CPU& cpu, bool skipped) {
		uint16_t* shifting = nullptr;
		if constexpr (std::is_same_v<SetMode, Mode::RegisterA>) {
			shifting = &cpu.regs.A;
		} else {
			shifting = &cpu.BufferOperand;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				if ( (get_lo(*shifting) & 0x80) != 0) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				uint8_t low = get_lo(*shifting);
				low = low << 1;
				*shifting = (get_hi(*shifting) << 8) | low;
				if ( (low & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (low == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				if ( (get_hi(*shifting) & 0x80) != 0) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				*shifting = (*shifting) << 1;
				if ( (get_hi(*shifting) & 0x80) != 0) {
					cpu.set_flag_N();
				}  else {
					cpu.clear_flag_N();
				}
				if (*shifting == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			if ( (get_lo(*shifting) & 0x80) != 0) {
				cpu.set_flag_C();
			} else {
				cpu.clear_flag_C();
			}
			uint8_t low = get_lo(*shifting);
			low = low << 1;
			*shifting = (get_hi(*shifting) << 8) | low;
			if ( (low & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (low == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode, typename SetMode = Mode::Operand>
	void ROL(CPU& cpu, bool skipped) {
		uint16_t* rotating = nullptr;
		if constexpr (std::is_same_v<SetMode, Mode::RegisterA>) {
			rotating = &cpu.regs.A;
		} else {
			rotating = &cpu.BufferOperand;
		}

		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				bool carry = cpu.get_flag_C();
				if ((get_lo(*rotating) & 0x80) != 0) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				uint8_t low = get_lo(*rotating);
				low = (low << 1);
				if (carry) {
					low = low | 0b1;
				}
				*rotating = (get_hi(*rotating) << 8) | low;
				if ((low & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (low == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				bool carry = cpu.get_flag_C();
				if ((get_hi(*rotating) & 0x80) != 0) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				*rotating = (*rotating << 1);
				if (carry) {
					*rotating = *rotating | 0b1;
				}
				if ((get_hi(*rotating) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (*rotating == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			bool carry = cpu.get_flag_C();
			if ((get_lo(*rotating) & 0x80) != 0) {
				cpu.set_flag_C();
			} else {
				cpu.clear_flag_C();
			}
			uint8_t low = get_lo(*rotating);
			low = (low << 1);
			if (carry) {
				low = low | 0b1;
			}
			*rotating = (get_hi(*rotating) << 8) | low;
			if ((low & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (low == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode, typename SetMode = Mode::Operand>
	void LSR(CPU& cpu, bool skipped) {
		uint16_t* shifting = nullptr;
		if constexpr (std::is_same_v<SetMode, Mode::RegisterA>) {
			shifting = &cpu.regs.A;
		} else {
			shifting = &cpu.BufferOperand;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				if ( (get_lo(*shifting) & 0x01) != 0) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				uint8_t low = get_lo(*shifting);
				low = low >> 1;
				*shifting = (get_hi(*shifting) << 8) | low;
				if ( (low & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (low == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				if ( (get_lo(*shifting) & 0x01) != 0) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				*shifting = (*shifting) >> 1;
				if ( (get_hi(*shifting) & 0x80) != 0) {
					cpu.set_flag_N();
				}  else {
					cpu.clear_flag_N();
				}
				if (*shifting == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			if ( (get_lo(*shifting) & 0x01) != 0) {
				cpu.set_flag_C();
			} else {
				cpu.clear_flag_C();
			}
			uint8_t low = get_lo(*shifting);
			low = low >> 1;
			*shifting = (get_hi(*shifting) << 8) | low;
			if ( (low & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (low == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode, typename SetMode = Mode::Operand>
	void ROR(CPU& cpu, bool skipped) {
		uint16_t* rotating = nullptr;
		if constexpr (std::is_same_v<SetMode, Mode::RegisterA>) {
			rotating = &cpu.regs.A;
		} else {
			rotating = &cpu.BufferOperand;
		}

		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				bool carry = cpu.get_flag_C();
				if ((get_lo(*rotating) & 0x01) != 0) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				uint8_t low = get_lo(*rotating);
				low = (low >> 1);
				if (carry) {
					low = low | 0x80;
				}
				*rotating = (get_hi(*rotating) << 8) | low;
				if ((low & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (low == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				bool carry = cpu.get_flag_C();
				if ((*rotating & 0x01) != 0) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				*rotating = (*rotating >> 1);
				if (carry) {
					*rotating = *rotating | 0x8000;
				}
				if ((get_hi(*rotating) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (*rotating == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			bool carry = cpu.get_flag_C();
			if ((get_lo(*rotating) & 0x01) != 0) {
				cpu.set_flag_C();
			} else {
				cpu.clear_flag_C();
			}
			uint8_t low = get_lo(*rotating);
			low = (low >> 1);
			if (carry) {
				low = low | 0x80;
			}
			*rotating = (get_hi(*rotating) << 8) | low;
			if ((low & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (low == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TSX(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_X()) {
				cpu.regs.X = (get_hi(cpu.regs.X) << 8) | (uint8_t)(get_lo(cpu.regs.S));
				if ((get_lo(cpu.regs.X) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(cpu.regs.X) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				cpu.regs.X = cpu.regs.S;
				if ((get_hi(cpu.regs.X) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (cpu.regs.X == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			cpu.regs.X = (get_hi(cpu.regs.X) << 8) | (uint8_t)(get_lo(cpu.regs.S));
			if ((get_lo(cpu.regs.X) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(cpu.regs.X) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TXY(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_X()) {
				cpu.regs.Y = (get_hi(cpu.regs.Y) << 8) | (uint8_t)(get_lo(cpu.regs.X));
				if ((get_lo(cpu.regs.Y) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(cpu.regs.Y) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				cpu.regs.Y = cpu.regs.X;
				if ((get_hi(cpu.regs.Y) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (cpu.regs.Y == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			cpu.regs.Y = (get_hi(cpu.regs.Y) << 8) | (uint8_t)(get_lo(cpu.regs.X));
			if ((get_lo(cpu.regs.Y) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(cpu.regs.Y) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TYX(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_X()) {
				cpu.regs.X = (get_hi(cpu.regs.X) << 8) | (uint8_t)(get_lo(cpu.regs.Y));
				if ((get_lo(cpu.regs.X) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(cpu.regs.X) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				cpu.regs.X = cpu.regs.Y;
				if ((get_hi(cpu.regs.X) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (cpu.regs.X == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			cpu.regs.X = (get_hi(cpu.regs.X) << 8) | (uint8_t)(get_lo(cpu.regs.Y));
			if ((get_lo(cpu.regs.X) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(cpu.regs.X) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TAX(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_X()) {
				cpu.regs.X = (get_hi(cpu.regs.X) << 8) | (uint8_t)(get_lo(cpu.regs.A));
				if ((get_lo(cpu.regs.X) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(cpu.regs.X) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				cpu.regs.X = cpu.regs.A;
				if ((get_hi(cpu.regs.X) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (cpu.regs.X == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			cpu.regs.X = (get_hi(cpu.regs.X) << 8) | (uint8_t)(get_lo(cpu.regs.A));
			if ((get_lo(cpu.regs.X) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(cpu.regs.X) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TAY(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_X()) {
				cpu.regs.Y = (get_hi(cpu.regs.Y) << 8) | (uint8_t)(get_lo(cpu.regs.A));
				if ((get_lo(cpu.regs.Y) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(cpu.regs.Y) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				cpu.regs.Y = cpu.regs.A;
				if ((get_hi(cpu.regs.Y) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (cpu.regs.Y == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			cpu.regs.Y = (get_hi(cpu.regs.Y) << 8) | (uint8_t)(get_lo(cpu.regs.A));
			if ((get_lo(cpu.regs.Y) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(cpu.regs.Y) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TXA(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				cpu.regs.A = (get_hi(cpu.regs.A) << 8) | (uint8_t)(get_lo(cpu.regs.X));
				if ((get_lo(cpu.regs.A) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(cpu.regs.A) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				cpu.regs.A = cpu.regs.X;
				if ((get_hi(cpu.regs.A) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (cpu.regs.A == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			cpu.regs.A = (get_hi(cpu.regs.A) << 8) | (uint8_t)(get_lo(cpu.regs.X));
			if ((get_lo(cpu.regs.A) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(cpu.regs.A) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TYA(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				cpu.regs.A = (get_hi(cpu.regs.A) << 8) | (uint8_t)(get_lo(cpu.regs.Y));
				if ((get_lo(cpu.regs.A) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(cpu.regs.A) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				cpu.regs.A = cpu.regs.Y;
				if ((get_hi(cpu.regs.A) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (cpu.regs.A == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			cpu.regs.A = (get_hi(cpu.regs.A) << 8) | (uint8_t)(get_lo(cpu.regs.Y));
			if ((get_lo(cpu.regs.A) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(cpu.regs.A) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TCD(CPU& cpu, bool skipped) {
		cpu.regs.D = cpu.regs.A;
		if ((get_hi(cpu.regs.D) & 0x80) != 0) {
			cpu.set_flag_N();
		} else {
			cpu.clear_flag_N();
		}
		if (cpu.regs.D == 0) {
			cpu.set_flag_Z();
		} else {
			cpu.clear_flag_Z();
		}
	}

	template <typename CPUMode>
	void TCS(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.S = cpu.regs.A;
		} else {
			cpu.regs.S = (0b1 << 8) | (uint8_t)(get_lo(cpu.regs.A));
		}
	}

	template <typename CPUMode>
	void TDC(CPU& cpu, bool skipped) {
		cpu.regs.A = cpu.regs.D;
		if ((get_hi(cpu.regs.A) & 0x80) != 0) {
			cpu.set_flag_N();
		} else {
			cpu.clear_flag_N();
		}
		if (cpu.regs.A == 0) {
			cpu.set_flag_Z();
		} else {
			cpu.clear_flag_Z();
		}
	}

	template <typename CPUMode>
	void TSC(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.A = cpu.regs.S;
			if ((get_hi(cpu.regs.A) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (cpu.regs.A == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		} else {
			cpu.regs.A = get_lo(cpu.regs.S) | 0x0100;
			if ((get_hi(cpu.regs.A) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (cpu.regs.A == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void TXS(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.S = cpu.regs.X;
		} else {
			cpu.regs.S = (0b1 << 8) | (uint8_t)(get_lo(cpu.regs.X));
		}
	}

	void XBA(CPU& cpu, bool skipped) {
		uint8_t lo = get_lo(cpu.regs.A);
		uint8_t hi = get_hi(cpu.regs.A);
		cpu.regs.A = (lo << 8) | (uint8_t)(hi);
		if ((get_lo(cpu.regs.A) & 0x80) != 0) {
			cpu.set_flag_N();
		} else {
			cpu.clear_flag_N();
		}
		if (get_lo(cpu.regs.A) == 0) {
			cpu.set_flag_Z();
		} else {
			cpu.clear_flag_Z();
		}
	}

	template <typename CPUMode, bool IsImmediate = false>
	void BIT(CPU& cpu, bool skipped) {
		if constexpr (IsImmediate) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_M()) {
				if ((get_lo(cpu.regs.A) & get_lo(cpu.BufferOperand)) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
				if constexpr (!IsImmediate) {
					if ((get_lo(cpu.BufferOperand) & 0x80) != 0) {
						cpu.set_flag_N();
					} else {
						cpu.clear_flag_N();
					}
					if ((get_lo(cpu.BufferOperand) & 0x40) != 0) {
						cpu.set_flag_V();
					} else {
						cpu.clear_flag_V();
					}
				}
			} else {
				if ((cpu.regs.A & cpu.BufferOperand) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
				if constexpr (!IsImmediate) {
					if ((get_hi(cpu.BufferOperand) & 0x80) != 0) {
						cpu.set_flag_N();
					} else {
						cpu.clear_flag_N();
					}
					if ((get_hi(cpu.BufferOperand) & 0x40) != 0) {
						cpu.set_flag_V();
					} else {
						cpu.clear_flag_V();
					}
				}
			}
		} else {
			if ((get_lo(cpu.regs.A) & get_lo(cpu.BufferOperand)) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
			if constexpr (!IsImmediate) {
				if ((get_lo(cpu.BufferOperand) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if ((get_lo(cpu.BufferOperand) & 0x40) != 0) {
					cpu.set_flag_V();
				} else {
					cpu.clear_flag_V();
				}
			}
		}
	}

	void JMPOp(CPU& cpu, bool skipped) {
		cpu.BufferPointer += cpu.regs.X;
		cpu.BufferBank = cpu.regs.PB;
	}

	void JMLDCRead(CPU& cpu, bool skipped) {
		Word address = cpu.BufferPointer + 2; 
	    Byte value_read = cpu.read(get_pcpb(address, 0));
	    cpu.BufferBank = value_read;
	}

	void PCOperandPBBank(CPU& cpu, bool skipped) {
		cpu.regs.PC = cpu.BufferOperand;
		cpu.regs.PB = cpu.BufferBank;
	}

	void PCAddressPBBank(CPU& cpu, bool skipped) {
		cpu.regs.PC = cpu.BufferAddress;
		cpu.regs.PB = cpu.BufferBank;
	}

	template <typename CPUMode, typename Direction, typename Changing, typename Flag>
	void INDE(CPU& cpu, bool skipped) {
		int8_t adding = 1;
		if constexpr (std::is_same_v<Direction, Mode::Decrease>) {
			adding = -1;
		}
		Word* changing = nullptr;
		if constexpr (std::is_same_v<Changing, Mode::RegisterA>) {
			changing = &cpu.regs.A;
		}
		if constexpr (std::is_same_v<Changing, Mode::RegisterX>) {
			changing = &cpu.regs.X;
		}
		if constexpr (std::is_same_v<Changing, Mode::RegisterY>) {
			changing = &cpu.regs.Y;
		}
		if constexpr (std::is_same_v<Changing, Mode::Operand>) {
			changing = &cpu.BufferOperand;
		}

		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			bool flag;
			if constexpr (std::is_same_v<Flag, Mode::MFlag>) {
				flag = cpu.get_flag_M();
			} else {
				flag = cpu.get_flag_X();
			}
			if (flag) {
				Byte low = get_lo(*changing);
				low += adding;
				*changing = (get_hi(*changing) << 8) | (uint8_t)(low);
				if ((get_lo(*changing) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(*changing) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				*changing = *changing + adding;
				if ((get_hi(*changing) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (*changing == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			Byte low = get_lo(*changing);
			low += adding;
			*changing = (get_hi(*changing) << 8) | (uint8_t)(low);
			if ((get_lo(*changing) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(*changing) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void REP(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.P = cpu.regs.P & ~get_lo(cpu.BufferOperand);
			if (cpu.get_flag_X()) {
				cpu.regs.X = cpu.regs.X & 0x00FF;
				cpu.regs.Y = cpu.regs.Y & 0x00FF;
			}
		} else {
			cpu.regs.P = cpu.regs.P & ~(get_lo(cpu.BufferOperand) & ~0x30);
		}
	}

	template <typename CPUMode>
	void SEP(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.P = cpu.regs.P | get_lo(cpu.BufferOperand);
			if (cpu.get_flag_X()) {
				cpu.regs.X = cpu.regs.X & 0x00FF;
				cpu.regs.Y = cpu.regs.Y & 0x00FF;
			}
		} else {
			cpu.regs.P = cpu.regs.P | (get_lo(cpu.BufferOperand) & ~0x30);
		}
	}

	template <typename CPUMode, typename Register, bool PCIncrement = false>
	void CopyRegister(CPU& cpu, bool skipped) {
		Word* reg = nullptr;
		if constexpr (std::is_same_v<Register, Mode::RegisterX>) {
			reg = &cpu.regs.X;
		} else {
			reg = &cpu.regs.Y;
		}
		if constexpr (PCIncrement) {
			cpu.regs.PC++;
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (cpu.get_flag_X()) {
				if (get_lo(*reg) >= get_lo(cpu.BufferOperand)) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				if (get_lo(*reg) == get_lo(cpu.BufferOperand)) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
				if (((get_lo(*reg) - get_lo(cpu.BufferOperand)) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
			} else {
				if (*reg >= cpu.BufferOperand) {
					cpu.set_flag_C();
				} else {
					cpu.clear_flag_C();
				}
				if (*reg == cpu.BufferOperand) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
				if (((*reg - cpu.BufferOperand) & 0x8000) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
			}
		} else {
			if (get_lo(*reg) >= get_lo(cpu.BufferOperand)) {
				cpu.set_flag_C();
			} else {
				cpu.clear_flag_C();
			}
			if (get_lo(*reg) == get_lo(cpu.BufferOperand)) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
			if (((get_lo(*reg) - get_lo(cpu.BufferOperand)) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
		}
	}

	void DecrementS(CPU& cpu, bool skipped) {
		if (!skipped) {
			cpu.regs.S -= 1;
		}
	}

	void DecrementSLow(CPU& cpu, bool skipped) {
		cpu.regs.S = (0b1 << 8) | (uint8_t)(get_lo(cpu.regs.S) - 1);
	}

	void DecrementS2(CPU& cpu, bool skipped) {
		if (!skipped) {
			cpu.regs.S -= 2;
		}
	}

	void DecrementS2Low(CPU& cpu, bool skipped) {
		cpu.regs.S = (0b1 << 8) | (uint8_t)(get_lo(cpu.regs.S) - 2);
	}

	void DecrementS2PCAddress(CPU& cpu, bool skipped) {
		if (!skipped) {
			cpu.regs.S -= 2;
			cpu.regs.PC = cpu.BufferAddress;
		}
	}

	void DecrementS2LowPCAddress(CPU& cpu, bool skipped) {
		cpu.regs.S = (0b1 << 8) | (uint8_t)(get_lo(cpu.regs.S) - 2);
		cpu.regs.PC = cpu.BufferAddress;
	}

	void IncrementSNativeAndReadBank(CPU& cpu, bool skipped) {
		Word address = cpu.regs.S + 1;
		cpu.BufferBank = cpu.read(get_pcpb(address, 0));
		cpu.regs.S = address;
	}

	void IncrementS(CPU& cpu, bool skipped) {
		if (!skipped) {
			cpu.regs.S += 1;
		}
	}

	void IncrementSLow(CPU& cpu, bool skipped) {
		cpu.regs.S = (0b1 << 8) | (uint8_t)(get_lo(cpu.regs.S) + 1);
	}

	void IncrementS2(CPU& cpu, bool skipped) {
		if (!skipped) {
			cpu.regs.S += 2;
		}
	}

	void IncrementS2Low(CPU& cpu, bool skipped) {
		cpu.regs.S = (0b1 << 8) | (uint8_t)(get_lo(cpu.regs.S) + 2);
	}

	void IncrementS2PCAddress(CPU& cpu, bool skipped) {
		if (!skipped) {
			cpu.regs.S += 2;
			cpu.regs.PC = cpu.BufferAddress;
		}
	}

	void IncrementS2LowPCAddress(CPU& cpu, bool skipped) {
		cpu.regs.S = (0b1 << 8) | (uint8_t)(get_lo(cpu.regs.S) + 2);
		cpu.regs.PC = cpu.BufferAddress;
	}

	template <typename CPUMode>
	void PHP(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.BufferOperand = (get_hi(cpu.BufferOperand) << 8) | (uint8_t)(cpu.regs.P);
		} else {
			uint8_t lo = cpu.regs.P | 0x30;
			cpu.BufferOperand = (get_hi(cpu.BufferOperand) << 8) | (uint8_t)(lo);
		}
	}

	template <typename CPUMode>
	void STXIndex(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>)  {
			cpu.BufferAddress = cpu.BufferOperand + cpu.regs.Y + cpu.regs.D;
		} else {
			if (get_lo(cpu.regs.D) == 0) {
				cpu.BufferAddress = (get_hi(cpu.BufferAddress) << 8) | (uint8_t)(cpu.BufferOperand + get_lo(cpu.regs.Y) + get_lo(cpu.regs.D));
				cpu.BufferAddress = (get_hi(cpu.regs.D) << 8) | get_lo(cpu.BufferAddress);
			} else {
				cpu.BufferAddress = cpu.BufferOperand + cpu.regs.Y + cpu.regs.D;
			}
		}
	}

	template <typename CPUMode>
	void STYIndex(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>)  {
			cpu.BufferAddress = cpu.BufferOperand + cpu.regs.X + cpu.regs.D;
		} else {
			if (get_lo(cpu.regs.D) == 0) {
				cpu.BufferAddress = (get_hi(cpu.BufferAddress) << 8) | (uint8_t)(cpu.BufferOperand + get_lo(cpu.regs.X) + get_lo(cpu.regs.D));
				cpu.BufferAddress = (get_hi(cpu.regs.D) << 8) | get_lo(cpu.BufferAddress);
			} else {
				cpu.BufferAddress = cpu.BufferOperand + cpu.regs.X + cpu.regs.D;
			}
		}
	}

	void JSRIndex(CPU& cpu, bool skipped) {
		cpu.BufferPointer += cpu.regs.X;
		cpu.BufferBank = cpu.regs.PB;
	}

	void PCAddress(CPU& cpu, bool skipped) {
		cpu.regs.PC = cpu.BufferAddress;
	}

	template <typename CPUMode>
	void JSL(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.S -= 3;
			cpu.regs.PC = cpu.BufferAddress;
			cpu.regs.PB = cpu.BufferBank;
		} else {
			cpu.regs.S = (0b1 << 8) | (get_lo(cpu.regs.S) - 3);
			cpu.regs.PC = cpu.BufferAddress;
			cpu.regs.PB = cpu.BufferBank;
		}
	}

	template <typename CPUMode, typename Register, typename Flag>
	void PL(CPU& cpu, bool skipped) {
		Word* reg = nullptr;
		if constexpr (std::is_same_v<Register, Mode::RegisterA>) {
			reg = &cpu.regs.A;
		} else if constexpr (std::is_same_v<Register, Mode::RegisterX>) {
			reg = &cpu.regs.X;
		} else {
			reg = &cpu.regs.Y;
		}
		bool flag = false;
		if constexpr (std::is_same_v<Flag, Mode::MFlag>) {
			flag = cpu.get_flag_M();
		} else {
			flag = cpu.get_flag_X();
		}
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			if (flag) {
				*reg = (get_hi(*reg) << 8) | get_lo(cpu.BufferOperand);
				if ((get_lo(*reg) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (get_lo(*reg) == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			} else {
				cpu.regs.S += 1;
				*reg = cpu.BufferOperand;
				if ((get_hi(*reg) & 0x80) != 0) {
					cpu.set_flag_N();
				} else {
					cpu.clear_flag_N();
				}
				if (*reg == 0) {
					cpu.set_flag_Z();
				} else {
					cpu.clear_flag_Z();
				}
			}
		} else {
			*reg = (get_hi(*reg) << 8) | get_lo(cpu.BufferOperand);
			if ((get_lo(*reg) & 0x80) != 0) {
				cpu.set_flag_N();
			} else {
				cpu.clear_flag_N();
			}
			if (get_lo(*reg) == 0) {
				cpu.set_flag_Z();
			} else {
				cpu.clear_flag_Z();
			}
		}
	}

	template <typename CPUMode>
	void PLP(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.P = get_lo(cpu.BufferOperand);
			if (cpu.get_flag_X()) {
				cpu.regs.X = cpu.regs.X & 0x00FF;
				cpu.regs.Y = cpu.regs.Y & 0x00FF;
			}
		} else {
			cpu.regs.P = get_lo(cpu.BufferOperand);
			cpu.set_flag_X();
			cpu.set_flag_M();
			cpu.regs.X = cpu.regs.X & 0x00FF;
			cpu.regs.Y = cpu.regs.Y & 0x00FF;	
		}
	}

	template <typename CPUMode>
	void PLB(CPU& cpu, bool skipped) {
		cpu.regs.DB = cpu.BufferBank;
		if ((cpu.regs.DB & 0x80) != 0) {
			cpu.set_flag_N();
		} else {
			cpu.clear_flag_N();
		}
		if (cpu.regs.DB == 0) {
			cpu.set_flag_Z();
		} else {
			cpu.clear_flag_Z();
		}
	}

	template <typename CPUMode>
	void PLD(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.S += 2;
		} else {
			cpu.regs.S = (0b1 << 8) | (get_lo(cpu.regs.S) + 2); 
		}
		cpu.regs.D = cpu.BufferOperand;
		if ((get_hi(cpu.regs.D) & 0x80) != 0) {
			cpu.set_flag_N();
		} else {
			cpu.clear_flag_N();
		}
		if (cpu.regs.D == 0) {
			cpu.set_flag_Z();
		} else {
			cpu.clear_flag_Z();
		}
	}

	template <typename CPUMode>
	void RTS(CPU& cpu, bool skipped) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			cpu.regs.S += 1;
		} else {
			cpu.regs.S = (0b1 << 8) | (get_lo(cpu.regs.S) + 1);
		}
		cpu.regs.PC = cpu.BufferOperand + 1;
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

	bool XFlagSet(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return cpu.get_flag_X();
	}

	template <typename CPUMode = Mode::Native>
	bool ReadingCondition(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			return (get_hi(cpu.BufferOrig) == get_hi(cpu.BufferPointer) && cpu.get_flag_X() == true);
		} else {
			return (get_hi(cpu.BufferOrig) == get_hi(cpu.BufferPointer));
		}
	}

	bool NoBranching(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return !cpu.Branching;
	}

	bool NoBoundaryCrossed(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return !cpu.BoundaryCrossed;
	}

	bool NoBranchingOrNoBoundaryCrossed(CPU& cpu) {
		PREDICATE_CHECK_ROUTINE
		return !cpu.Branching || !cpu.BoundaryCrossed;
	}
}

// ORA (01)
Instruction n_01 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_01 = {
	EMULATION_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// ORA (03)
Instruction n_03 = {
	NATIVE_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_03 = {
	EMULATION_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// TSB (04)
Instruction n_04 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand, Mode::PlusOne>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::TB<Mode::Native, Mode::Set>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::AddressPlusOne>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_04 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::TB<Mode::Emulation, Mode::Set>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// ORA (05)
Instruction n_05 = {
	NATIVE_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_05 = {
	EMULATION_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// ASL (06)
Instruction n_06 = {
	NATIVE_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Native>),
	NATIVE_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_06 = {
	EMULATION_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Emulation>),
	EMULATION_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// ORA (07)
Instruction n_07 = {
	NATIVE_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_07 = {
	EMULATION_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// PHP (08)
Instruction n_08 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::PHP<Mode::Native>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	NEXT_OPCODE
};
Instruction e_08 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::PHP<Mode::Emulation>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::Stack0Emulation>),
	MakeHandler(Ricoh5A22Functions::DecrementSLow),
	NEXT_OPCODE
};

// ORA (09)
Instruction n_09 = {
	NATIVE_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_09 = {
	EMULATION_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// ASL (0A)
Instruction n_0a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Native, Mode::RegisterA>),
	NEXT_OPCODE
};
Instruction e_0a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Emulation, Mode::RegisterA>),
	NEXT_OPCODE
};

// PHD (0B)
Instruction n_0b = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::DHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::DLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2),
	NEXT_OPCODE
};
Instruction e_0b = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::DHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::DLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2Low),
	NEXT_OPCODE
};

// TSB (0C)
Instruction n_0c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::AddressDB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::AddressPlusOneDBCarry, ReadTo::OperandHigh>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::TB<Mode::Native, Mode::Set>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::AddressPlusOneDBCarry>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_0c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::AddressDB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::TB<Mode::Emulation, Mode::Set>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// ORA (0D)
Instruction n_0d = {
	NATIVE_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_0d = {
	EMULATION_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// ASL (0E)
Instruction n_0e = {
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Native>),
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_0e = {
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Emulation>),
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// ORA (0F)
Instruction n_0f = {
	NATIVE_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_0f = {
	EMULATION_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// BPL (10)
Instruction n_10 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::N_Zero>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_10 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::N_Zero>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// ORA (11)
Instruction n_11 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_11 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// ORA (12)
Instruction n_12 = {
	NATIVE_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_12 = {
	EMULATION_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// ORA (13)
Instruction n_13 = {
	NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_13 = {
	EMULATION_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// TRB (14)
Instruction n_14 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand, Mode::PlusOne>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::TB<Mode::Native, Mode::Reset>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::AddressPlusOne>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_14 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::TB<Mode::Emulation, Mode::Reset>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// ORA (15)
Instruction n_15 = {
	NATIVE_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_15 = {
	EMULATION_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// ASL (16)
Instruction n_16 = {
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Native>),
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_16 = {
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Emulation>),
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// ORA (17)
Instruction n_17 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_17 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// CLC (18)
Instruction n_18 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::CLC),
	NEXT_OPCODE
};
Instruction e_18 = n_18;

// ORA (19)
Instruction n_19 = {
	NATIVE_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_19 = {
	EMULATION_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// INC (1A)
Instruction n_1a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Increase, Mode::RegisterA, Mode::MFlag>),
	NEXT_OPCODE
};
Instruction e_1a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Increase, Mode::RegisterA, Mode::MFlag>),
	NEXT_OPCODE
};

// TCS (1B)
Instruction n_1b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TCS<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_1b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TCS<Mode::Emulation>),
	NEXT_OPCODE
};


// TRB (1C)
Instruction n_1c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::AddressDB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::AddressPlusOneDBCarry, ReadTo::OperandHigh>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::TB<Mode::Native, Mode::Reset>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::AddressPlusOneDBCarry>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_1c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::AddressDB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::TB<Mode::Emulation, Mode::Reset>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// ORA (1D)
Instruction n_1d = {
	NATIVE_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_1d = {
	EMULATION_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// ASL (1E)
Instruction n_1e = {
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Native>),
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_1e = {
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ASL<Mode::Emulation>),
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// ORA (1F)
Instruction n_1f = {
	NATIVE_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_1f = {
	EMULATION_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::ORA<Mode::Emulation>),
	NEXT_OPCODE
};

// JSR (20)
Instruction n_20 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::DecrementPC),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2PCAddress),
	NEXT_OPCODE
};
Instruction e_20 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::DecrementPC),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCHigh, WriteTo::Stack0Emulation>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1Emulation>),
	MakeHandler(Ricoh5A22Functions::DecrementS2LowPCAddress),
	NEXT_OPCODE
};

// AND (21)
Instruction n_21 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_21 = {
	EMULATION_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// JSL (22)
Instruction n_22 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PB, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Bank>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCHigh, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCLow, WriteTo::StackMinus2>),
	MakeHandler(Ricoh5A22Functions::JSL<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_22 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PB, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Bank>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCHigh, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCLow, WriteTo::StackMinus2>),
	MakeHandler(Ricoh5A22Functions::JSL<Mode::Emulation>),
	NEXT_OPCODE
};

// AND (23)
Instruction n_23 = {
	NATIVE_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_23 = {
	EMULATION_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// BIT (24)
Instruction n_24 = {
	NATIVE_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_24 = {
	EMULATION_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Emulation>),
	NEXT_OPCODE
};

// AND (25)
Instruction n_25 = {
	NATIVE_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_25 = {
	EMULATION_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// ROL (26)
Instruction n_26 = {
	NATIVE_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Native>),
	NATIVE_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_26 = {
	EMULATION_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Emulation>),
	EMULATION_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// AND (27)
Instruction n_27 = {
	NATIVE_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_27 = {
	EMULATION_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// PLP (28)
Instruction n_28 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementS),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::PLP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_28 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementSLow),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0Emulation, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::PLP<Mode::Emulation>),
	NEXT_OPCODE
};

// AND (29)
Instruction n_29 = {
	NATIVE_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_29 = {
	EMULATION_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// ROL (2A)
Instruction n_2a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Native, Mode::RegisterA>),
	NEXT_OPCODE
};
Instruction e_2a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Emulation, Mode::RegisterA>),
	NEXT_OPCODE
};

// PLD (2B)
Instruction n_2b = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack1, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack2, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::PLD<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_2b = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack1, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack2, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::PLD<Mode::Emulation>),
	NEXT_OPCODE
};

// BIT (2C)
Instruction n_2c = {
	NATIVE_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_2c = {
	EMULATION_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Emulation>),
	NEXT_OPCODE
};

// AND (2D)
Instruction n_2d = {
	NATIVE_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_2d = {
	EMULATION_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// ROL (2E)
Instruction n_2e = {
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Native>),
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_2e = {
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Emulation>),
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// AND (2F)
Instruction n_2f = {
	NATIVE_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_2f = {
	EMULATION_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// BMI (30)
Instruction n_30 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::N_One>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_30 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::N_One>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// AND (31)
Instruction n_31 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_31 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// AND (32)
Instruction n_32 = {
	NATIVE_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_32 = {
	EMULATION_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// AND (33)
Instruction n_33 = {
	NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_33 = {
	EMULATION_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// BIT (34)
Instruction n_34 = {
	NATIVE_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_34 = {
	EMULATION_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Emulation>),
	NEXT_OPCODE
};

// AND (35)
Instruction n_35 = {
	NATIVE_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_35 = {
	EMULATION_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// ROL (36)
Instruction n_36 = {
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Native>),
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_36 = {
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Emulation>),
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// AND (37)
Instruction n_37 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_37 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// SEC (38)
Instruction n_38 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::SEC),
	NEXT_OPCODE
};
Instruction e_38 = n_38;

// AND (39)
Instruction n_39 = {
	NATIVE_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_39 = {
	EMULATION_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// DEC (3A)
Instruction n_3a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Decrease, Mode::RegisterA, Mode::MFlag>),
	NEXT_OPCODE
};
Instruction e_3a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Decrease, Mode::RegisterA, Mode::MFlag>),
	NEXT_OPCODE
};

// TSC (3B)
Instruction n_3b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TSC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_3b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TSC<Mode::Emulation>),
	NEXT_OPCODE
};

// BIT (3C)
Instruction n_3c = {
	NATIVE_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_3c = {
	EMULATION_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Emulation>),
	NEXT_OPCODE
};

// AND (3D)
Instruction n_3d = {
	NATIVE_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_3d = {
	EMULATION_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// ROL (3E)
Instruction n_3e = {
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Native>),
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_3e = {
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROL<Mode::Emulation>),
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// AND (3F)
Instruction n_3f = {
	NATIVE_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_3f = {
	EMULATION_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::AND<Mode::Emulation>),
	NEXT_OPCODE
};

// EOR (41)
Instruction n_41 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_41 = {
	EMULATION_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// WDM (42)
Instruction n_42 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	NEXT_OPCODE
};
Instruction e_42 = n_42;

// EOR (43)
Instruction n_43 = {
	NATIVE_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_43 = {
	EMULATION_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// MVP (44)
Instruction n_44 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::DBOL>),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Bank>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::XBank, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::YDB>),
	MakeHandler(Ricoh5A22Functions::MVP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_44 = n_44;


// EOR (45)
Instruction n_45 = {
	NATIVE_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_45 = {
	EMULATION_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// LSR (46)
Instruction n_46 = {
	NATIVE_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Native>),
	NATIVE_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_46 = {
	EMULATION_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Emulation>),
	EMULATION_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// EOR (47)
Instruction n_47 = {
	NATIVE_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_47 = {
	EMULATION_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// PHA (48)
Instruction n_48 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::AHigh, WriteTo::Stack0>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::ALow, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	NEXT_OPCODE
};
Instruction e_48 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::ALow, WriteTo::Stack0Emulation>),
	MakeHandler(Ricoh5A22Functions::DecrementSLow),
	NEXT_OPCODE
};

// EOR (49)
Instruction n_49 = {
	NATIVE_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_49 = {
	EMULATION_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// LSR (4A)
Instruction n_4a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Native, Mode::RegisterA>),
	NEXT_OPCODE
};
Instruction e_4a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Emulation, Mode::RegisterA>),
	NEXT_OPCODE
};

// PHK (4B)
Instruction n_4b = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PB, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	NEXT_OPCODE
};
Instruction e_4b = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PB, WriteTo::Stack0Emulation>),
	MakeHandler(Ricoh5A22Functions::DecrementSLow),
	NEXT_OPCODE
};

// JMP (4C)
Instruction n_4c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::Copy<ReadFrom::Address, ReadTo::PC>),
	NEXT_OPCODE
};
Instruction e_4c = n_4c;

// EOR (4D)
Instruction n_4d = {
	NATIVE_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_4d = {
	EMULATION_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// LSR (4E)
Instruction n_4e = {
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Native>),
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_4e = {
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Emulation>),
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// EOR (4F)
Instruction n_4f = {
	NATIVE_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_4f = {
	EMULATION_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// BVC (50)
Instruction n_50 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::V_Zero>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_50 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::V_Zero>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// EOR (51)
Instruction n_51 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_51 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// EOR (52)
Instruction n_52 = {
	NATIVE_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_52 = {
	EMULATION_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// EOR (53)
Instruction n_53 = {
	NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_53 = {
	EMULATION_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// MVN (54)
Instruction n_54 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::DBOL>),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Bank>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::XBank, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::YDB>),
	MakeHandler(Ricoh5A22Functions::MVN),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_54 = n_54;

// EOR (55)
Instruction n_55 = {
	NATIVE_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_55 = {
	EMULATION_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// LSR (56)
Instruction n_56 = {
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Native>),
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_56 = {
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Emulation>),
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// EOR (57)
Instruction n_57 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_57 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// CLI (58)
Instruction n_58 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::CLI),
	NEXT_OPCODE
};
Instruction e_58 = n_58;

// EOR (59)
Instruction n_59 = {
	NATIVE_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_59 = {
	EMULATION_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// PHY (5A)
Instruction n_5a = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YHigh, WriteTo::Stack0>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YLow, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	NEXT_OPCODE
};
Instruction e_5a = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YLow, WriteTo::Stack0Emulation>),
	MakeHandler(Ricoh5A22Functions::DecrementSLow),
	NEXT_OPCODE
};

// TCD (5B)
Instruction n_5b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TCD<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_5b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TCD<Mode::Emulation>),
	NEXT_OPCODE
};

// JML (5C)
Instruction n_5c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Bank>),
	MakeHandler(Ricoh5A22Functions::PCAddressPBBank),
	NEXT_OPCODE
};
Instruction e_5c = n_5c;

// EOR (5D)
Instruction n_5d = {
	NATIVE_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_5d = {
	EMULATION_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// LSR (5E)
Instruction n_5e = {
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Native>),
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_5e = {
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::LSR<Mode::Emulation>),
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// EOR (5F)
Instruction n_5f = {
	NATIVE_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_5f = {
	EMULATION_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::EOR<Mode::Emulation>),
	NEXT_OPCODE
};

// RTS (60)
Instruction n_60 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementS),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack1, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::RTS<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_60 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementSLow),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack1, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::RTS<Mode::Emulation>),
	NEXT_OPCODE
};

// ADC (61)
Instruction n_61 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_61 = {
	EMULATION_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// PER (62)
Instruction n_62 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OOPC>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2),
	NEXT_OPCODE
};
Instruction e_62 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OOPC>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2Low),
	NEXT_OPCODE
};

// ADC (63)
Instruction n_63 = {
	NATIVE_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_63 = {
	EMULATION_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// STZ (64)
Instruction n_64 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::Zero, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::Zero, WriteTo::AddressPlusOne>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_64 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::Zero, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE	
};

// ADC (65)
Instruction n_65 = {
	NATIVE_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_65 = {
	EMULATION_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// ROR (66)
Instruction n_66 = {
	NATIVE_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Native>),
	NATIVE_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_66 = {
	EMULATION_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Emulation>),
	EMULATION_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// ADC (67)
Instruction n_67 = {
	NATIVE_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_67 = {
	EMULATION_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// PLA (68)
Instruction n_68 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementS),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack1, ReadTo::OperandHigh>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::PL<Mode::Native, Mode::RegisterA, Mode::MFlag>),
	NEXT_OPCODE
};
Instruction e_68 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementSLow),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::PL<Mode::Emulation, Mode::RegisterA, Mode::MFlag>),
	NEXT_OPCODE
};

// ADC (69)
Instruction n_69 = {
	NATIVE_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_69 = {
	EMULATION_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// ROR (6A)
Instruction n_6a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Native, Mode::RegisterA>),
	NEXT_OPCODE
};
Instruction e_6a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Emulation, Mode::RegisterA>),
	NEXT_OPCODE
};

// JMP (6C)
Instruction n_6c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::PointerHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Pointer, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerPlusOne, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::Copy<ReadFrom::Address, ReadTo::PC>),
	NEXT_OPCODE
};
Instruction e_6c = n_6c;

// ADC (6D)
Instruction n_6d = {
	NATIVE_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_6d = {
	EMULATION_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// ROR (6E)
Instruction n_6e = {
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Native>),
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_6e = {
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Emulation>),
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// ADC (6F)
Instruction n_6f = {
	NATIVE_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_6f = {
	EMULATION_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// BVS (70)
Instruction n_70 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::V_One>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_70 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::V_One>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// ADC (71)
Instruction n_71 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_71 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// ADC (72)
Instruction n_72 = {
	NATIVE_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_72 = {
	EMULATION_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// ADC (73)
Instruction n_73 = {
	NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_73 = {
	EMULATION_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// STZ (74)
Instruction n_74 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::STYIndex<Mode::Native>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::Zero, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::Zero, WriteTo::AddressPlusOne>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_74 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::STYIndex<Mode::Emulation>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::Zero, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// ADC (75)
Instruction n_75 = {
	NATIVE_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_75 = {
	EMULATION_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// ROR (76)
Instruction n_76 = {
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Native>),
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_76 = {
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Emulation>),
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// ADC (77)
Instruction n_77 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_77 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// SEI (78)
Instruction n_78 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::SEI),
	NEXT_OPCODE
};
Instruction e_78 = n_78;


// ADC (79)
Instruction n_79 = {
	NATIVE_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_79 = {
	EMULATION_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// PLY (7A)
Instruction n_7a = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementS),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack1, ReadTo::OperandHigh>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::PL<Mode::Native, Mode::RegisterY, Mode::XFlag>),
	NEXT_OPCODE
};
Instruction e_7a = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementSLow),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::PL<Mode::Emulation, Mode::RegisterY, Mode::XFlag>),
	NEXT_OPCODE
};

// TDC (7B)
Instruction n_7b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TDC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_7b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TDC<Mode::Emulation>),
	NEXT_OPCODE
};

// JMP (7C)
Instruction n_7c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::PointerHigh>),
	MakeHandler(Ricoh5A22Functions::JMPOp),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerBank, ReadTo::AddressLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerPlusOneBankNoCarry, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::Copy<ReadFrom::Address, ReadTo::PC>),
	NEXT_OPCODE
};
Instruction e_7c = n_7c;

// ADC (7D)
Instruction n_7d = {
	NATIVE_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_7d = {
	EMULATION_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};

// ROR (7E)
Instruction n_7e = {
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Native>),
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_7e = {
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::ROR<Mode::Emulation>),
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// ADC (7F)
Instruction n_7f = {
	NATIVE_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_7f = {
	EMULATION_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::ADC<Mode::Emulation>),
	NEXT_OPCODE
};


// BRA (80)
Instruction n_80 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::Always>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_80 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::Always>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// STA (81)
Instruction n_81 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_81 = {
	EMULATION_DIRECT_INDEXED_INDIRECT_D_X_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// BRL (82)
Instruction n_82 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::BRL),
	NEXT_OPCODE
};
Instruction e_82 = n_82;

// STA (83)
Instruction n_83 = {
	NATIVE_STACK_RELATIVE_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_83 = {
	EMULATION_STACK_RELATIVE_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STY (84)
Instruction n_84 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YHigh, WriteTo::AddressPlusOne>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_84 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE	
};

// STA (85)
Instruction n_85 = {
	NATIVE_DIRECT_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_85 = {
	EMULATION_DIRECT_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STX (86)
Instruction n_86 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XHigh, WriteTo::AddressPlusOne>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_86 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE	
};

// STA (87)
Instruction n_87 = {
	NATIVE_DIRECT_INDIRECT_LONG_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_87 = {
	EMULATION_DIRECT_INDIRECT_LONG_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// DEY (88)
Instruction n_88 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Decrease, Mode::RegisterY, Mode::XFlag>),
	NEXT_OPCODE
};
Instruction e_88 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Decrease, Mode::RegisterY, Mode::XFlag>),
	NEXT_OPCODE
};

// TXA (8A)
Instruction n_8a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TXA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_8a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TXA<Mode::Emulation>),
	NEXT_OPCODE
};

// PHB (8B)
Instruction n_8b = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::DB, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	NEXT_OPCODE
};
Instruction e_8b = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::DB, WriteTo::Stack0Emulation>),
	MakeHandler(Ricoh5A22Functions::DecrementSLow),
	NEXT_OPCODE
};

// STY (8C)
Instruction n_8c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OY>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::AddressPlusOneDBCarry>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_8c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OY>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STA (8D)
Instruction n_8d = {
	NATIVE_ABSOLUTE_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_8d = {
	EMULATION_ABSOLUTE_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STX (8E)
Instruction n_8e = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OX>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::AddressPlusOneDBCarry>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_8e = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OX>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STA (8F)
Instruction n_8f = {
	NATIVE_ABSOLUTE_LONG_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_8f = {
	EMULATION_ABSOLUTE_LONG_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// BIT (89)
Instruction n_89 = {
	NATIVE_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Native, Mode::IsImmediate>),
	NEXT_OPCODE
};
Instruction e_89 = {
	EMULATION_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::BIT<Mode::Emulation, Mode::IsImmediate>),
	NEXT_OPCODE
};

// BCC (90)
Instruction n_90 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::C_Zero>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_90 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::C_Zero>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// STA (91)
Instruction n_91 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_91 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_D_Y_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STA (92)
Instruction n_92 = {
	NATIVE_DIRECT_INDIRECT_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_92 = {
	EMULATION_DIRECT_INDIRECT_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STA (93)
Instruction n_93 = {
	NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_93 = {
	EMULATION_STACK_RELATIVE_INDIRECT_INDEXED_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STY (94)
Instruction n_94 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::STYIndex<Mode::Native>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YHigh, WriteTo::AddressPlusOne>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_94 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::STYIndex<Mode::Emulation>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::YLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STA (95)
Instruction n_95 = {
	NATIVE_DIRECT_X_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_95 = {
	EMULATION_DIRECT_X_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STX (96)
Instruction n_96 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::STXIndex<Mode::Native>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XHigh, WriteTo::AddressPlusOne>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_96 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::STXIndex<Mode::Emulation>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XLow, WriteTo::Address>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STA (97)
Instruction n_97 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_97 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_LONG_D_Y_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// TYA (98)
Instruction n_98 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TYA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_98 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TYA<Mode::Emulation>),
	NEXT_OPCODE
};


// STA (99)
Instruction n_99 = {
	NATIVE_ABSOLUTE_Y_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_99 = {
	EMULATION_ABSOLUTE_Y_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// TXS (9A)
Instruction n_9a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TXS<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_9a = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TXS<Mode::Emulation>),
	NEXT_OPCODE
};

// TXY (9B)
Instruction n_9b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TXY<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_9b = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TXY<Mode::Emulation>),
	NEXT_OPCODE
};

// STZ (9C)
Instruction n_9c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OZ>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::AddressPlusOneDBCarry>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_9c = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Address>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OZ>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::AddressDB>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STA (9D)
Instruction n_9d = {
	NATIVE_ABSOLUTE_X_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_9d = {
	EMULATION_ABSOLUTE_X_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STZ (9E)
Instruction n_9e = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::PointerHigh>),
	MakeHandler(Ricoh5A22Functions::AbsoluteLongXIndex<Mode::PCIncrement>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OZ, false, BranchMode::None, true>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::PointerBank>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::PointerPlusOneBankCarry>, Ricoh5A22Predicates::MFlagSet),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_9e = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::PointerHigh>),
	MakeHandler(Ricoh5A22Functions::AbsoluteLongXIndex<Mode::PCIncrement>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::OZ, false, BranchMode::None, true>),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::PointerBank>),
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// STA (9F)
Instruction n_9f = {
	NATIVE_ABSOLUTE_LONG_X_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_9f = {
	EMULATION_ABSOLUTE_LONG_X_WRITE
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};

// LDY (A0)
Instruction n_a0 = {
	NATIVE_IMMEDIATE_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_a0 = {
	EMULATION_IMMEDIATE_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// LDA (A1)
Instruction n_a1 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_a1 = {
	EMULATION_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// LDX (A2)
Instruction n_a2 = {
	NATIVE_IMMEDIATE_X
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_a2 = {
	EMULATION_IMMEDIATE_X
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// LDA (A3)
Instruction n_a3 = {
	NATIVE_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_a3 = {
	EMULATION_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// LDY (A4)
Instruction n_a4 = {
	NATIVE_DIRECT_READ_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_a4 = {
	EMULATION_DIRECT_READ_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Emulation>),
	NEXT_OPCODE
};


// LDA (A5)
Instruction n_a5 = {
	NATIVE_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_a5 = {
	EMULATION_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// LDX (A6)
Instruction n_a6 = {
	NATIVE_DIRECT_READ_X
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_a6 = {
	EMULATION_DIRECT_READ_X
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (A7)
Instruction n_a7 = {
	NATIVE_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_a7 = {
	EMULATION_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};

// TAY (A8)
Instruction n_a8 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TAY<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_a8 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TAY<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (A9)
Instruction n_a9 = {
	NATIVE_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_a9 = {
	EMULATION_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// TAX (AA)
Instruction n_aa = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TAX<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_aa = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TAX<Mode::Emulation>),
	NEXT_OPCODE
};

// PLB (AB)
Instruction n_ab = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementS),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::Bank>),
	MakeHandler(Ricoh5A22Functions::PLB<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_ab = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementSNativeAndReadBank),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::PLB<Mode::Emulation>),
	NEXT_OPCODE
};

// LDY (AC)
Instruction n_ac = {
	NATIVE_ABSOLUTE_READ_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_ac = {
	EMULATION_ABSOLUTE_READ_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (AD)
Instruction n_ad = {
	NATIVE_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_ad = {
	EMULATION_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// LDX (AE)
Instruction n_ae = {
	NATIVE_ABSOLUTE_READ_X
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_ae = {
	EMULATION_ABSOLUTE_READ_X
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (AF)
Instruction n_af = {
	NATIVE_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_af = {
	EMULATION_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// BCS (B0)
Instruction n_b0 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::C_One>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_b0 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::C_One>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// LDA (B1)
Instruction n_b1 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_b1 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};

// LDA (B2)
Instruction n_b2 = {
	NATIVE_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_b2 = {
	EMULATION_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (B3)
Instruction n_b3 = {
	NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_b3 = {
	EMULATION_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// LDY (B4)
Instruction n_b4 = {
	NATIVE_DIRECT_X_READ_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_b4 = {
	EMULATION_DIRECT_X_READ_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (B5)
Instruction n_b5 = {
	NATIVE_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_b5 = {
	EMULATION_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// LDX (B6)
Instruction n_b6 = {
	NATIVE_DIRECT_Y_READ
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_b6 = {
	EMULATION_DIRECT_Y_READ
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (B7)
Instruction n_b7 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_b7 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// CLV (B8)
Instruction n_b8 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::CLV),
	NEXT_OPCODE
};
Instruction e_b8 = n_b8;


// LDA (B9)
Instruction n_b9 = {
	NATIVE_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_b9 = {
	EMULATION_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// TSX (BA)
Instruction n_ba = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TSX<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_ba = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TSX<Mode::Emulation>),
	NEXT_OPCODE
};

// TYX (BB)
Instruction n_bb = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TYX<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_bb = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::TYX<Mode::Emulation>),
	NEXT_OPCODE
};

// LDY (BC)
Instruction n_bc = {
	NATIVE_ABSOLUTE_X_READ_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_bc = {
	EMULATION_ABSOLUTE_X_READ_X
	MakeHandler(Ricoh5A22Functions::LDY<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (BD)
Instruction n_bd = {
	NATIVE_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_bd = {
	EMULATION_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// LDX (BE)
Instruction n_be = {
	NATIVE_ABSOLUTE_Y_READ_X
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_be = {
	EMULATION_ABSOLUTE_Y_READ_X
	MakeHandler(Ricoh5A22Functions::LDX<Mode::Emulation>),
	NEXT_OPCODE
};

// LDA (BF)
Instruction n_bf = {
	NATIVE_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_bf = {
	EMULATION_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::LDA<Mode::Emulation>),
	NEXT_OPCODE
};

// CPY (C0)
Instruction n_c0 = {
	NATIVE_IMMEDIATE_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Native, Mode::RegisterY, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_c0 = {
	EMULATION_IMMEDIATE_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Emulation, Mode::RegisterY, Mode::PCIncrement>),
	NEXT_OPCODE
};

// CMP (C1)
Instruction n_c1 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_c1 = {
	EMULATION_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// REP (C2)
Instruction n_c2 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::REP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_c2 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::REP<Mode::Emulation>),
	NEXT_OPCODE
};

// CMP (C3)
Instruction n_c3 = {
	NATIVE_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_c3 = {
	EMULATION_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// CPY (C4)
Instruction n_c4 = {
	NATIVE_DIRECT_READ_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Native, Mode::RegisterY>),
	NEXT_OPCODE
};
Instruction e_c4 = {
	EMULATION_DIRECT_READ_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Emulation, Mode::RegisterY>),
	NEXT_OPCODE
};

// CMP (C5)
Instruction n_c5 = {
	NATIVE_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_c5 = {
	EMULATION_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// DEC (C6)
Instruction n_c6 = {
	NATIVE_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Decrease, Mode::Operand, Mode::MFlag>),
	NATIVE_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_c6 = {
	EMULATION_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Decrease, Mode::Operand, Mode::MFlag>),
	EMULATION_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// CMP (C7)
Instruction n_c7 = {
	NATIVE_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_c7 = {
	EMULATION_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// INY (C8)
Instruction n_c8 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Increase, Mode::RegisterY, Mode::XFlag>),
	NEXT_OPCODE
};
Instruction e_c8 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Increase, Mode::RegisterY, Mode::XFlag>),
	NEXT_OPCODE
};

// CMP (C9)
Instruction n_c9 = {
	NATIVE_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_c9 = {
	EMULATION_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// DEX (CA)
Instruction n_ca = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Decrease, Mode::RegisterX, Mode::XFlag>),
	NEXT_OPCODE
};
Instruction e_ca = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Decrease, Mode::RegisterX, Mode::XFlag>),
	NEXT_OPCODE
};

// WAI (CB)
Instruction n_cb = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22SpecialFunctions::WAIT),
};
Instruction e_cb = n_cb;

// CPY (CC)
Instruction n_cc = {
	NATIVE_ABSOLUTE_READ_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Native, Mode::RegisterY>),
	NEXT_OPCODE
};
Instruction e_cc = {
	EMULATION_ABSOLUTE_READ_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Emulation, Mode::RegisterY>),
	NEXT_OPCODE
};

// CMP (CD)
Instruction n_cd = {
	NATIVE_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_cd = {
	EMULATION_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// DEC (CE)
Instruction n_ce = {
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Decrease, Mode::Operand, Mode::MFlag>),
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_ce = {
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Decrease, Mode::Operand, Mode::MFlag>),
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// CMP (CF)
Instruction n_cf = {
	NATIVE_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_cf = {
	EMULATION_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// BNE (D0)
Instruction n_d0 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::Z_Zero>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_d0 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::Z_Zero>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// CMP (D1)
Instruction n_d1 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_d1 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// CMP (D2)
Instruction n_d2 = {
	NATIVE_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_d2 = {
	EMULATION_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// CMP (D3)
Instruction n_d3 = {
	NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_d3 = {
	EMULATION_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// PEI (D4)
Instruction n_d4 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand, Mode::PlusOne>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2),
	NEXT_OPCODE
};
Instruction e_d4 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::DLZero),
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::AOD, true>),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Address, ReadTo::Operand, Mode::PlusOne>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2Low),
	NEXT_OPCODE	
};

// CMP (D5)
Instruction n_d5 = {
	NATIVE_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_d5 = {
	EMULATION_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// DEC (D6)
Instruction n_d6 = {
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Decrease, Mode::Operand, Mode::MFlag>),
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_d6 = {
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Decrease, Mode::Operand, Mode::MFlag>),
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// CMP (D7)
Instruction n_d7 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_d7 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// CLD (D8)
Instruction n_d8 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::CLD),
	NEXT_OPCODE
};
Instruction e_d8 = n_d8;


// CMP (D9)
Instruction n_d9 = {
	NATIVE_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_d9 = {
	EMULATION_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// PHX (DA)
Instruction n_da = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XHigh, WriteTo::Stack0>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XLow, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::DecrementS),
	NEXT_OPCODE
};
Instruction e_da = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::XLow, WriteTo::Stack0Emulation>),
	MakeHandler(Ricoh5A22Functions::DecrementSLow),
	NEXT_OPCODE
};

// STP (DB)
Instruction n_db = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22SpecialFunctions::STOP),
};
Instruction e_db = n_db;

// JML (DC)
Instruction n_dc = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::PointerHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Pointer, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerPlusOne, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::JMLDCRead),
	MakeHandler(Ricoh5A22Functions::PCOperandPBBank),
	NEXT_OPCODE
};
Instruction e_dc = n_dc;

// CMP (DD)
Instruction n_dd = {
	NATIVE_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_dd = {
	EMULATION_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// DEC (DE)
Instruction n_de = {
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Decrease, Mode::Operand, Mode::MFlag>),
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_de = {
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Decrease, Mode::Operand, Mode::MFlag>),
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// CMP (DF)
Instruction n_df = {
	NATIVE_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_df = {
	EMULATION_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::CMP<Mode::Emulation>),
	NEXT_OPCODE
};

// CPX (E0)
Instruction n_e0 = {
	NATIVE_IMMEDIATE_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Native, Mode::RegisterX, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_e0 = {
	EMULATION_IMMEDIATE_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Emulation, Mode::RegisterX, Mode::PCIncrement>),
	NEXT_OPCODE
};

// SBC (E1)
Instruction n_e1 = {
	NATIVE_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_e1 = {
	EMULATION_DIRECT_INDEXED_INDIRECT_D_X_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// SEP (E2)
Instruction n_e2 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::SEP<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_e2 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::SEP<Mode::Emulation>),
	NEXT_OPCODE
};

// SBC (E3)
Instruction n_e3 = {
	NATIVE_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_e3 = {
	EMULATION_STACK_RELATIVE_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// CPX (E4)
Instruction n_e4 = {
	NATIVE_DIRECT_READ_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Native, Mode::RegisterX>),
	NEXT_OPCODE
};
Instruction e_e4 = {
	EMULATION_DIRECT_READ_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Emulation, Mode::RegisterX>),
	NEXT_OPCODE
};

// SBC (E5)
Instruction n_e5 = {
	NATIVE_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_e5 = {
	EMULATION_DIRECT_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// INC (E6)
Instruction n_e6 = {
	NATIVE_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Increase, Mode::Operand, Mode::MFlag>),
	NATIVE_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_e6 = {
	EMULATION_DIRECT_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Increase, Mode::Operand, Mode::MFlag>),
	EMULATION_DIRECT_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// SBC (E7)
Instruction n_e7 = {
	NATIVE_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_e7 = {
	EMULATION_DIRECT_INDIRECT_LONG_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// INX (E8)
Instruction n_e8 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Increase, Mode::RegisterX, Mode::XFlag>),
	NEXT_OPCODE
};
Instruction e_e8 = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Increase, Mode::RegisterX, Mode::XFlag>),
	NEXT_OPCODE
};

// SBC (E9)
Instruction n_e9 = {
	NATIVE_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native, Mode::PCIncrement>),
	NEXT_OPCODE
};
Instruction e_e9 = {
	EMULATION_IMMEDIATE_M
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation, Mode::PCIncrement>),
	NEXT_OPCODE
};

// NOP (EA)
Instruction n_ea = {
	IMPLIED
	MakeHandler(Ricoh5A22Functions::NOP),
	NEXT_OPCODE
};
Instruction e_ea = n_ea;

// XBA (EB)
Instruction n_eb = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::XBA),
	NEXT_OPCODE
};
Instruction e_eb = n_eb;

// CPX (EC)
Instruction n_ec = {
	NATIVE_ABSOLUTE_READ_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Native, Mode::RegisterX>),
	NEXT_OPCODE
};
Instruction e_ec = {
	EMULATION_ABSOLUTE_READ_X
	MakeHandler(Ricoh5A22Functions::CopyRegister<Mode::Emulation, Mode::RegisterX>),
	NEXT_OPCODE
};

// SBC (ED)
Instruction n_ed = {
	NATIVE_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_ed = {
	EMULATION_ABSOLUTE_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// INC (EE)
Instruction n_ee = {
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Increase, Mode::Operand, Mode::MFlag>),
	NATIVE_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_ee = {
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Increase, Mode::Operand, Mode::MFlag>),
	EMULATION_ABSOLUTE_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// SBC (EF)
Instruction n_ef = {
	NATIVE_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_ef = {
	EMULATION_ABSOLUTE_LONG_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// BEQ (F0)
Instruction n_f0 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::Z_One>),
	NATIVE_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};
Instruction e_f0 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC<SetMode::None, false, BranchMode::Z_One>),
	EMULATION_FLAG_BRANCH_LOGIC
	NEXT_OPCODE
};

// SBC (F1)
Instruction n_f1 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_f1 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_D_Y_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// SBC (F2)
Instruction n_f2 = {
	NATIVE_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_f2 = {
	EMULATION_DIRECT_INDIRECT_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// SBC (F3)
Instruction n_f3 = {
	NATIVE_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_f3 = {
	EMULATION_STACK_RELATIVE_INDIRECT_INDEXED_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// PEA (F4)
Instruction n_f4 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2),
	NEXT_OPCODE
};
Instruction e_f4 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Operand>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::OperandHigh>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::OperandLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2Low),
	NEXT_OPCODE
};

// SBC (F5)
Instruction n_f5 = {
	NATIVE_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_f5 = {
	EMULATION_DIRECT_X_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// INC (F6)
Instruction n_f6 = {
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Increase, Mode::Operand, Mode::MFlag>),
	NATIVE_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_f6 = {
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Increase, Mode::Operand, Mode::MFlag>),
	EMULATION_DIRECT_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// SBC (F7)
Instruction n_f7 = {
	NATIVE_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_f7 = {
	EMULATION_DIRECT_INDIRECT_INDEXED_LONG_D_Y_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// SED (F8)
Instruction n_f8 = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::SED),
	NEXT_OPCODE
};
Instruction e_f8 = n_f8;


// SBC (F9)
Instruction n_f9 = {
	NATIVE_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_f9 = {
	EMULATION_ABSOLUTE_Y_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// PLX (FA)
Instruction n_fa = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementS),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::NOP, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack1, ReadTo::OperandHigh>, Ricoh5A22Predicates::XFlagSet),
	MakeHandler(Ricoh5A22Functions::PL<Mode::Native, Mode::RegisterX, Mode::XFlag>),
	NEXT_OPCODE
};
Instruction e_fa = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::IncrementSLow),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::Stack0, ReadTo::OperandLow>),
	MakeHandler(Ricoh5A22Functions::PL<Mode::Emulation, Mode::RegisterX, Mode::XFlag>),
	NEXT_OPCODE
};

// XCE (FB)
Instruction n_fb = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Discard>),
	MakeHandler(Ricoh5A22Functions::XCE),
	NEXT_OPCODE
};
Instruction e_fb = n_fb;

// JSR (FC)
Instruction n_fc = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCHigh, WriteTo::Stack0>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1>),
	MakeHandler(Ricoh5A22Functions::DecrementS2),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::PointerHigh>),
	MakeHandler(Ricoh5A22Functions::JSRIndex),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerBank, ReadTo::AddressLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerPlusOneBankNoCarry, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::PCAddress),
	NEXT_OPCODE
};
Instruction e_fc = {
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::Pointer>),
	MakeHandler(Ricoh5A22Functions::IncrementPC),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCHigh, WriteTo::Stack0Emulation>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Write<WriteValue::PCLow, WriteTo::StackMinus1Emulation>),
	MakeHandler(Ricoh5A22Functions::DecrementS2Low),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PCPB, ReadTo::PointerHigh>),
	MakeHandler(Ricoh5A22Functions::JSRIndex),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerBank, ReadTo::AddressLow>),
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::Read<ReadFrom::PointerPlusOneBankNoCarry, ReadTo::AddressHigh>),
	MakeHandler(Ricoh5A22Functions::PCAddress),
	NEXT_OPCODE
};

// SBC (FD)
Instruction n_fd = {
	NATIVE_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_fd = {
	EMULATION_ABSOLUTE_X_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

// INC (FE)
Instruction n_fe = {
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Native, Mode::Increase, Mode::Operand, Mode::MFlag>),
	NATIVE_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};
Instruction e_fe = {
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_START
	MakeHandler(Ricoh5A22Functions::INDE<Mode::Emulation, Mode::Increase, Mode::Operand, Mode::MFlag>),
	EMULATION_ABSOLUTE_X_READ_MODIFY_WRITE_END
	NEXT_OPCODE
};

// SBC (FF)
Instruction n_ff = {
	NATIVE_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Native>),
	NEXT_OPCODE
};
Instruction e_ff = {
	EMULATION_ABSOLUTE_LONG_X_READ
	MakeHandler(Ricoh5A22Functions::SBC<Mode::Emulation>),
	NEXT_OPCODE
};

Instruction nop = {
	MakeHandler(Ricoh5A22Functions::NOP),
	MakeHandler(Ricoh5A22Functions::NOP),
};