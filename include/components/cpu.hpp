#pragma once
#include <functional>
#include <type_traits>

#include "component.hpp"

class CPU : public Component {
public:
	
	struct Registers {

		// Both
		Word A;
		Word X;
		Word Y;
		Word PC;
		Word S;
		Byte P;

		// Ricoh 5A22

		Word D;
		Byte DB;
		Byte PB;
		bool emulation_mode;
		
	};

	Registers regs;

	Word BufferOpCode;
	Word BufferPointer;
	Word BufferAddress;
	Word BufferOperand;

	bool condition;

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
	
	virtual Byte read(Address addr) = 0;
	virtual void write(Address addr, Byte value) = 0;

	virtual void enable_test_mode() = 0;
	virtual void disable_test_mode() = 0;
	virtual void reset_test_memory() = 0;
	virtual Byte test_peek(Address addr) = 0;
	virtual void test_poke(Address addr, Byte value) = 0;
};