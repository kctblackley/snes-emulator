#pragma once
#include "cpu.hpp"

// Handler stores pointer to half-cycle instruction

// bool is for handling skipped 

template <class CpuT>
using HandlerFn = void(*)(CpuT&, bool);

template <class CpuT>
using PredicateFn = bool(*)(CpuT&);

template <class CpuT>
struct Handler {
	HandlerFn<CpuT> function;
	PredicateFn<CpuT> predicate;
};

template<class CpuT>
constexpr Handler<CpuT> MakeHandler(
    void(*fn)(CpuT&, bool),
    bool(*pred)(CpuT&) = nullptr)
{
    return {fn, pred};
}

template <class CpuT>
using Instruction = std::vector<Handler<CpuT>>;

template <class CpuT>
using Optable = std::array<Instruction<CpuT>*, 258>;

template <class CpuT>
struct Opcode {
	HandlerFn<CpuT> function;
	CycleCount idx;
	bool skipped;
};

template <class CpuT>
Opcode<CpuT> get_opcode(const Optable<CpuT>& optable, Word opcode, CycleCount& idx, CpuT& cpu) {
	Instruction<CpuT>& instruction = *optable[opcode];
	
	Handler<CpuT>* handler = nullptr;
	bool skipped = false;
	bool predicate = true;
	while(predicate && idx < instruction.size()) {
		handler = &instruction[idx++];
		predicate = handler->predicate ? handler->predicate(cpu) : false;
		if (predicate) {
			skipped = true;
		}
	}

	if (idx >= instruction.size() ) { idx = 0; }

	return Opcode{ handler->function, idx, skipped };
}