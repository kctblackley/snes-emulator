#include "opcode_engine.hpp"

Opcode get_opcode(const Optable& optable, Byte opcode, CycleCount& idx, CPU& cpu) {
	Instruction& instruction = *optable[opcode];
	
	Handler* handler = nullptr;
	bool skipped = false;
	bool predicate = true;
	while(predicate && idx < instruction.size()) {
		handler = &instruction[idx++];
		predicate = handler->predicate(cpu);
		if (predicate) {
			skipped = true;
		}
	}

	if (idx >= instruction.size() ) { idx = 0; }

	Opcode op = { handler->function, idx, skipped };
	return op;
}