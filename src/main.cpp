#include "snes.hpp"
#include "test_harness.hpp"
#include "ricoh_5a22_native_optable.hpp"
#include "ricoh_5a22_emulation_optable.hpp"

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

int main() {
	// --- Opcode single-step tests ---------------------------------------
	// Drop a SingleStepTests-style JSON file at tests/<name>.json and call
	// test("<name>", <opcode byte>) to check it against your implementation.
	// e.g. tests/a1_n.json + opcode 0xA1 (LDA (dp,X)):
	if constexpr(SST_TEST) {
		Byte instruction = 0x00;
		int implemented = 0;
		int implemented_n = 0;
		int implemented_e = 0;
		int incorrect = 0;
		int incorrect_n = 0;
		int incorrect_e = 0;
		bool passed;
		std::cout << COLOR_BOLD << "Native mode\n";
		for (auto o : native_optable) {
			if (o != &nop) {
				passed = test(opcode_to_filename(instruction, false), instruction);
				if (!passed) {
					incorrect++;
					incorrect_n++;
				}
				implemented++;
				implemented_n++;
			}
			instruction++;
		}
		if (implemented_n < 256) {
			if (implemented_n == 0) {
				std::cout << COLOR_RED << "No implementations completed.\n";
			} else {
				std::cout << "Implementations done: " << implemented_n << " out of 256" << "\n";
			}
		} else {
			std::cout << COLOR_GREEN << "All implementations wired up.\n";
		}
		if (incorrect_n > 0) {
			std::cout << COLOR_RED << "Incorrect implementations: " << incorrect_n << "\n";
		}

		std::cout << "\n";
		std::cout << COLOR_BOLD << "Emulation mode\n";
		for (auto o : emulation_optable) {
			if (o != &nop) {
				passed = test(opcode_to_filename(instruction, true), instruction);
				if (!passed) {
					incorrect++;
					incorrect_e++;
				}
				implemented++;
				implemented_e++;
			}
			instruction++;
		}
		if (implemented_e < 256) {
			if (implemented_e == 0) {
				std::cout << COLOR_RED << "No implementations completed.\n";
			} else {
				std::cout << "Implementations done: " << implemented_e << " out of 256" << "\n";
			}
		} else {
			std::cout << COLOR_GREEN << "All implementations wired up.\n";
		}
		if (incorrect_e > 0) {
			std::cout << COLOR_RED << "Incorrect implementations: " << incorrect_e << "\n";
		}

		std::cout << "\n";
		std::cout << COLOR_BOLD << "Completion Status\n";
		if (implemented == 512 && incorrect == 0) {
			std::cout << COLOR_GREEN << "Ricoh 5A22 CPU completed. Hooray!\n";
		} else {
			std::cout << COLOR_YELLOW << "Incomplete Ricoh 5A22 CPU. Keep working.\n";
			int progress_percentage = (int)(100.0f * ((float)implemented / 512.0f));
			std::cout << COLOR_YELLOW << progress_percentage << "% per-cent finished\n";
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