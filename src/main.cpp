#include "snes.hpp"
#include "test_harness.hpp"
#include "ricoh_5a22_optable.hpp"

std::string opcode_to_filename(uint8_t opcode) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%02x.n", opcode);
    return std::string(buf);
}

int main() {
	// --- Opcode single-step tests ---------------------------------------
	// Drop a SingleStepTests-style JSON file at tests/<name>.json and call
	// test("<name>", <opcode byte>) to check it against your implementation.
	// e.g. tests/a1_n.json + opcode 0xA1 (LDA (dp,X)):
	if constexpr(SST_TEST) {
		Byte instruction = 0x00;
		int operations_implemented = 0;
		int incorrect_operations = 0;
		bool passed;
		for (auto o : optable) {
			if (o != &nop) {
				passed = test(opcode_to_filename(instruction), instruction);
				if (!passed) {
					incorrect_operations++;
				}
				operations_implemented++;
			}
			instruction++;
		}
		std::cout << "Implemented " << operations_implemented << " operation(s).\n";
		if (incorrect_operations > 0) {
			std::cout << incorrect_operations << " implementation(s) are incorrect. Fix these first before continuing.\n";
		}
		if (operations_implemented < 256) {
			std::cout << "This instruction set is incomplete.\n";
		}
		if (incorrect_operations == 0 && operations_implemented == 256) {
			std::cout << "The instruction set is finished. Hooray!\n";
		}
		//test("a5.n", 0xa5);
		return 0;
	}

	// --- Normal emulation -------------------------------------------------
	std::string directory = "rom/tales_of_phantasia.sfc";

	SNES snes = SNES();
	snes.initialise();
	snes.load_cartridge(directory);
	snes.run();
}