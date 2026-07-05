#pragma once
#include <functional>

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
	
	virtual Byte read(Address addr) = 0;
	virtual void write(Address addr, Byte value) = 0;
};