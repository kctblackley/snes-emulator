#include "snes.hpp"

int main() {
	SNES snes = SNES();
	

	snes.load("rom/");
	snes.run();
}