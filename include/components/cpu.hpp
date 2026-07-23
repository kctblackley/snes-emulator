#pragma once
#include <functional>
#include <type_traits>

#include "component.hpp"

class CPU : public Component {
public:
	
	struct Registers {

		// Both
		Word A = 0;
		Word X = 0;
		Word Y = 0;
		Word PC = 0;
		Word S = 0;
		Byte P = 0;

		// Ricoh 5A22

		Word D = 0;
		Byte DB = 0;
		Byte PB = 0;
		bool emulation_mode = false;
		
	};

	Registers regs;

	Word BufferOpCode = 0;
	Word BufferPointer = 0;
	Word BufferAddress = 0;
	Word BufferOperand = 0;
	Word BufferOperand0 = 0;
	Word BufferOperand1 = 0;
	Word BufferBank = 0;
	Word BufferOrig = 0;
	Word BufferMVDest = 0;
	Word BufferStackAddress = 0;
	Word Vector = 0;

	Word YABuffer = 0; // just a buffer, does not give actual value of YA for SPC-700
	Word BufferJump = 0;

	uint32_t DivYa = 0;
	uint32_t ShiftedX = 0;

	Word TransferCount = 0;

	Word DiscardBuffer = 0;

	bool condition = 0;

	bool Branching = 0;
	bool BoundaryCrossed = 0;

	virtual void apply_invariants() = 0;
	virtual void poll_interrupts() = 0;

	virtual bool get_flag_N() { return false; }
	virtual bool get_flag_V() { return false; }
	virtual bool get_flag_M() { return false; }
	virtual bool get_flag_X() { return false; }
	virtual bool get_flag_D() { return false; }
	virtual bool get_flag_I() { return false; }
	virtual bool get_flag_Z() { return false; }
	virtual bool get_flag_C() { return false; }
	virtual bool get_flag_P() { return false; }
	virtual bool get_flag_H() { return false; }
	virtual bool get_flag_B() { return false; }
	
	virtual void set_flag_N(Byte value) = 0;
	virtual void set_flag_V(Byte value) = 0;
	virtual void set_flag_M(Byte value) = 0;
	virtual void set_flag_X(Byte value) = 0;
	virtual void set_flag_D(Byte value) = 0;
	virtual void set_flag_I(Byte value) = 0;
	virtual void set_flag_Z(Word value) = 0;
	virtual void set_flag_C(Byte value) = 0;
	virtual void set_flag_P(Byte value) = 0;
	virtual void set_flag_H(Byte value) = 0;
	virtual void set_flag_B(Byte value) = 0;

	virtual void set_flag_N() = 0;
	virtual void set_flag_V() = 0;
	virtual void set_flag_M() = 0;
	virtual void set_flag_X() = 0;
	virtual void set_flag_D() = 0;
	virtual void set_flag_I() = 0;
	virtual void set_flag_Z() = 0;
	virtual void set_flag_C() = 0;
	virtual void set_flag_P() = 0;
	virtual void set_flag_H() = 0;
	virtual void set_flag_B() = 0;

	virtual void clear_flag_N() = 0;
	virtual void clear_flag_V() = 0;
	virtual void clear_flag_M() = 0;
	virtual void clear_flag_X() = 0;
	virtual void clear_flag_D() = 0;
	virtual void clear_flag_I() = 0;
	virtual void clear_flag_Z() = 0;
	virtual void clear_flag_C() = 0;
	virtual void clear_flag_P() = 0;
	virtual void clear_flag_H() = 0;
	virtual void clear_flag_B() = 0;
	
	virtual void enable_test_mode() = 0;
	virtual void disable_test_mode() = 0;
	virtual void reset_test_memory() = 0;
	virtual Byte test_peek(Address addr) = 0;
	virtual void test_poke(Address addr, Byte value) = 0;
};