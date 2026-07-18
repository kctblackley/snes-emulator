#include "common.hpp"

Byte set_bit(Byte byte, Byte bit) {
	return byte | (0b1 << bit);
}

Byte clear_bit(Byte byte, Byte bit) {
	return byte & ~(0b1 << bit);
}
