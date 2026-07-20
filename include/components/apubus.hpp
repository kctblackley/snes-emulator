#pragma once
#include <array>

#define APU_DATA_SIZE 65536
#define IPL_SIZE 64

class APUBus {
public:
	APUBus() {}

	Byte read(Word address) {
		Byte fetched = data[address];
		if (ipl_enabled && address >= 0xFFC0) {
			fetched = ipl[address - 0xFFC0];
		}

		return fetched;
	}

	void write(Word address, Byte value) {
		data[address] = value;
	}

	void enable_ipl()  { ipl_enabled = true; }
	void disable_ipl() { ipl_enabled = false; }

	// --- Test-harness hooks -------------------------------------------
	// The APU's 64KB space is just a flat array already (no cartridge-style
	// restrictions to bypass), so "test mode" only needs to make sure the
	// IPL ROM overlay doesn't shadow raw RAM contents, and that each test
	// case starts from a clean slate.
	void enable_test_mode()  { disable_ipl(); }
	void disable_test_mode() { return; }

	void reset_test_memory() {
		data.fill(0);
		ipl_enabled = false;
	}

private:

	bool ipl_enabled = false;
	std::array<Byte, APU_DATA_SIZE> data {};

	std::array<Byte, IPL_SIZE> ipl {
		0xCD, 0xEF, 0xBD, 0xE8, 0x00, 0xC6, 0x1D, 0xD0, 0xFC, 0x8F, 0xAA, 0xF4, 0x8F, 0xBB, 0xF5, 0x78,
        0xCC, 0xF4, 0xD0, 0xFB, 0x2F, 0x19, 0xEB, 0xF4, 0xD0, 0xFC, 0x7E, 0xF4, 0xD0, 0x0B, 0xE4, 0xF5,
        0xCB, 0xF4, 0xD7, 0x00, 0xFC, 0xD0, 0xF3, 0xAB, 0x01, 0x10, 0xEF, 0x7E, 0xF4, 0x10, 0xEB, 0xBA,
        0xF6, 0xDA, 0x00, 0xBA, 0xF4, 0xC4, 0xF4, 0xDD, 0x5D, 0xD0, 0xDB, 0x1F, 0x00, 0x00, 0xC0, 0xFF
	};

};