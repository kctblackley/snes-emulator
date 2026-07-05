#include "snes.hpp"

int main() {
	std::string directory = "rom/super_mario_world.sfc";

	SNES snes = SNES();
	snes.initialise();
	snes.load_cartridge(directory);
	snes.run();
}