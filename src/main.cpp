#include "snes.hpp"
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

bool to_test(Byte instruction) {
	std::vector<Byte> instructions = {0x00};
	return (std::count(instructions.begin(), instructions.end(), instruction) > 0);
}

int main() {
	std::string directory = "rom/super_mario_world.sfc";

	SNES snes = SNES();
	snes.load_cartridge(directory);
	snes.run();
}