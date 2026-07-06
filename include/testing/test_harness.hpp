#pragma once
#include <string>

#include "common.hpp"

// Runs every single-step test case found in tests/<opcode_name>.json against
// the given opcode.
//
//   opcode_name  - the stem of the test file, e.g. "a1_n" loads tests/a1_n.json
//   opcode_value - the actual opcode byte to dispatch through the optable, e.g. 0xA1
//
// For each test case, CPU registers and memory are set up from the case's
// "initial" block, the instruction is run to completion via the existing
// Handler/Instruction machinery, and the resulting registers + touched memory
// are compared against the case's "final" block. Results are printed to
// stdout as PASSED/FAILED lines followed by a summary. Cycle-exactness is not
// checked, only whether "initial" correctly becomes "final".
bool test(const std::string& opcode_name, Byte opcode_value);
