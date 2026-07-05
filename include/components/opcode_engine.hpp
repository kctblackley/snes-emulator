#pragma once
#include "cpu.hpp"
#include <functional>

// Handler stores pointer to half-cycle instruction

// bool is for handling skipped 

using HandlerFn = void(*)(CPU&, bool);
using PredicateFn = bool(*)(CPU&);

struct Handler {
	HandlerFn function;
	PredicateFn predicate;
};

inline bool DefaultPredicate(CPU&) {
	return false;
}
constexpr Handler MakeHandler(HandlerFn fn, PredicateFn pred = DefaultPredicate) {
	return Handler{fn, pred};
}

using Instruction = std::vector<Handler>;
using Optable = std::array<Instruction*, 256>;

struct Opcode {
	HandlerFn function;
	CycleCount idx;
	bool skipped;
};

Opcode get_opcode(const Optable& optable, Byte opcode, CycleCount& idx, CPU& cpu);