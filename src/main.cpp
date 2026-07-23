#include "snes.hpp"
#include "spc_700_optable.hpp"
#include "ricoh_5a22_native_optable.hpp"
#include "ricoh_5a22_emulation_optable.hpp"
#include "test_harness.hpp"
#include <string>
#include <iomanip>
#include <sstream>

std::string opcode_to_filename(uint8_t opcode, bool is_emulation = false) {
    char buf[8];
    if (is_emulation) {
    	std::snprintf(buf, sizeof(buf), "%02x.e", opcode);
    } else {
    	std::snprintf(buf, sizeof(buf), "%02x.n", opcode);
    }
    return std::string(buf);
}

constexpr const char* COLOR_GREEN  = "\033[32m";
constexpr const char* COLOR_RED    = "\033[31m";
constexpr const char* COLOR_YELLOW = "\033[33m";
constexpr const char* COLOR_BOLD   = "\033[1m";
constexpr const char* COLOR_RESET  = "\033[0m";

bool to_test(Byte instruction) {
	std::vector<Byte> instructions = {0x00};
	return (std::count(instructions.begin(), instructions.end(), instruction) > 0);
}

int main(int argc, char* argv[]) {
	// --- SPC-700 opcode single-step tests ---------------------------------
	// Drop a SingleStepTests-style JSON file at tests/<name>.json and call
	// test_spc700("<name>", <opcode byte>) to check it against your implementation.
	// e.g. tests/00.json + opcode 0x00 (NOP):
	
	
	// TEST HANDLER

	/*int total = 0;
	for (int i = 0; i < optable.size(); i++) {
		if (optable[i] != &s_nop) {
			std::stringstream ss;
			ss << std::hex
			   << std::setw(2)
			   << std::setfill('0')
			   << i;
			total++;
			test_spc700(ss.str(), i);
		}
	}

	std::cout << total << " instructions implemented.\n";
	return 0;*/

	// --- Ricoh 5A22 opcode single-step tests -------------------------------
	// test("a1_n", 0xA1);

	// --- Normal emulation ---------------------------------------------------
	std::string directory = std::string("rom/") + argv[1] + ".sfc";
	SNES snes = SNES();
	snes.load_cartridge(directory);
	snes.run();
}