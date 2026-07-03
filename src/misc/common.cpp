#include "common.hpp"

SNESAddress split_address(Address address) {
	Bank bank = (address & 0xFF0000) >> 16;
	Offset offset = address & 0xFFFF;
	return {bank, offset};
}

Quadrant get_quadrant(Bank bank) {
	if (bank  <  0x40) { return 1; }
	if (bank  <  0x80) { return 2; }
	if (bank  <  0xC0) { return 3; }
	if (bank  <= 0xFF) { return 4; }
}
