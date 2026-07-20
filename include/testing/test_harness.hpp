#pragma once
#include <string>

#include "common.hpp"

// Runs every single-step test case found in tests/<opcode_name>.json against
// the given Ricoh 5A22 opcode.
//
//   opcode_name  - the stem of the test file, e.g. "a1_n" loads tests/a1_n.json
//   opcode_value - the actual opcode byte to dispatch through the optable, e.g. 0xA1
//
// For each test case, CPU registers and memory are set up from the case's
// "initial" block, the instruction is run to completion via the existing
// Handler/Instruction machinery, and the resulting registers + touched memory
// are compared against the case's "final" block. Results are printed to
// stdout as PASSED/FAILED lines followed by a summary. Cycle-exactness is not
// checked, only whether "initial" correctly becomes "final". Returns true iff
// every test case passed.
bool test(const std::string& opcode_name, Byte opcode_value);

// Same idea as test(), but for the SPC-700 (the SNES's sound CPU) instead of
// the Ricoh 5A22. Loads tests/<opcode_name>.json, expecting the SingleStepTests
// SPC-700 register/ram layout (pc, a, x, y, sp, psw + ram pairs, no bank
// registers). Returns true iff every test case passed.
bool test_spc700(const std::string& opcode_name, Byte opcode_value);

