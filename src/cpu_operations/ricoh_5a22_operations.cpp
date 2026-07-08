#include "ricoh_5a22_operations.hpp"
#include "ricoh_5a22_addressing_modes.hpp"

// Micro-instructions
// Functions are all of the type

namespace ReadTo {
	struct OpCode  {};
	struct Pointer {};
	struct Address {};
	struct Operand {};
	struct Bank    {};

	struct PC {};

	struct OpCodeHigh  {};
	struct PointerHigh {};
	struct AddressHigh {};
	struct OperandHigh {};
	struct BankHigh    {};

	struct PCPB      {};
	struct PointerDB {};
}

namespace ReadFrom {
	struct OpCode    {};
	struct Pointer   {};
	struct Address   {};
	struct AddressDL {};
	struct Operand   {};

	struct PC {};

	struct AddressPlusOne   {};
	struct AddressPlusOneDL {};

	struct AddressPlusTwo   {};
	struct AddressPlusTwoDL {};
	
	struct PointerBank    {};
	struct PointerPlusOneBankCarry {};
	struct PointerDB    {};
	struct PointerPlusOneDBCarry {};
	struct AddressDB               {};
	struct AddressPlusOneDBCarry   {};
	struct AddressBank             {};
	struct AddressPlusOneBankCarry {};
	struct PCPB      {};
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

	// Emulation
	struct DXEmulation   {};
	struct APSEmulation  {};

	struct AOXDEmulation {};
}

namespace Mode {
	constexpr bool PlusOne = true;
	constexpr bool PCIncrement = true;
	constexpr bool IfSkipped = true;

	struct Native    {};
	struct Emulation {};
}

namespace BranchMode {
	struct None   {};
	struct N_Zero  {};
	struct N_One   {};
	struct V_Zero  {};
	struct V_One   {};
	struct Always {};
	struct C_Zero  {};
	struct C_One   {};
	struct Z_Zero  {};
	struct Z_One   {};
}

Address get_pcpb(Word pc, Byte pb) {
	return (pb << 16) | pc;
}

namespace Ricoh5A22Functions {
	void NOP(CPU& cpu, bool skipped) {
		return;
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
		if constexpr (std::is_same_v<From, ReadFrom::PointerPlusOneBankCarry>) {
			register_offset = cpu.BufferPointer + 1;
			register_bank = cpu.BufferBank;

			if (cpu.BufferPointer == 0xFFFF) {
				register_bank++;
			}
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
		if constexpr (std::is_same_v<To, ReadTo::Bank> || std::is_same_v<To, ReadTo::BankHigh>) {
			read_to = &cpu.BufferBank;
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

	template <typename From, typename To, typename Mode = CopyMode::All>
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

	template <typename CPUMode = Mode::Native>
	bool ReadingCondition(CPU& cpu) {
		if constexpr (std::is_same_v<CPUMode, Mode::Native>) {
			return (get_hi(cpu.BufferOrig) == get_hi(cpu.BufferPointer) && cpu.get_flag_X() == true);
		} else {
			return (get_hi(cpu.BufferOrig) == get_hi(cpu.BufferPointer));
		}
	}

	bool NoBranching(CPU& cpu) {
		return !cpu.Branching;
	}

	bool NoBoundaryCrossed(CPU& cpu) {
		return !cpu.BoundaryCrossed;
	}

	bool NoBranchingOrNoBoundaryCrossed(CPU& cpu) {
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

// ADC (6d)
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