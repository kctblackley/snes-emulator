#include "snes.hpp"

int main() {
	SNES snes = SNES();
	
	std::vector<Byte> rom {};
	
	if constexpr (SST_TEST) {
		std::string rom_text = "3e d 12";
		rom = load_rom_text(rom_text);
	} else {
		std::string directory = "rom/super_mario_world.sfc";
		rom = load_rom_file(directory);
	}

	snes.initialise();
	snes.load(rom);
	snes.run();
}