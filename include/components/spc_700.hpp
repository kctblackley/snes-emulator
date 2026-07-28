#pragma once
#include "cpu.hpp"
#include "spc_700_optable.hpp"
#include "common.hpp"
#include "apubus.hpp"

#define SPC_700_CYCLE_CONSTANT 20.9738991477

enum class APUStubState {
	WaitForCC,
	Transfer
};

struct SPCTimer {
	bool enabled = false;

	Byte target = 0;
	Byte output = 0;

	uint16_t divider_counter = 0;
	uint16_t internal_counter = 0;
};

class SPC700 : public CPU {
public:
	SPC700();

	void add_cycles(CycleCount cycles) override;

	void run_half_cycle();
	void accumulate(CycleCount delta);
	void tick_component() override;
	CycleCount get_cycle() override;
	TickCount get_tick() override;

	void reset() override;
	void initialise() override;
	
	Byte communication_read(SNESAddress addr) override {
		return spc_to_cpu_ports[(addr.offset - 0x2140) & 3];
	}

	void communication_write(SNESAddress addr, Byte value) override {
		cpu_to_spc_ports[(addr.offset - 0x2140) & 3] = value;
	}

	Byte read(Address addr) override {
		if (addr >= 0xFD && addr <= 0xFF) {
			Byte value = timers[addr - 0xFD].output;
			timers[addr - 0xFD].output = 0;
			return value;
		}
		if (addr >= 0xF4 && addr <= 0xF7) {
			return cpu_to_spc_ports[addr - 0xF4];
		}
		if (ipl_rom_enabled && addr >= 0xFFC0) {
			return ipl_rom[addr - 0xFFC0];
		}
		return bus->read(addr);
	}

	void write(Address addr, Byte value) override {
		if (addr >= 0xFA && addr <= 0xFC) {
			timers[addr - 0xFA].target = value;
			return;
		}
		if (addr == 0xF1) {

		    bool old0 = timers[0].enabled;
		    bool old1 = timers[1].enabled;
		    bool old2 = timers[2].enabled;

		    timers[0].enabled = value & 0x01;
		    timers[1].enabled = value & 0x02;
		    timers[2].enabled = value & 0x04;

		    if (!old0 && timers[0].enabled) {
			    timers[0].output = 0;
			    timers[0].divider_counter = 0;
			    timers[0].internal_counter = 0;
			}

		    if (!old1 && timers[1].enabled) {
			    timers[1].output = 0;
			    timers[1].divider_counter = 0;
			    timers[1].internal_counter = 0;
			}

		    if (!old2 && timers[2].enabled) {
			    timers[2].output = 0;
			    timers[2].divider_counter = 0;
			    timers[2].internal_counter = 0;
			}

		    ipl_rom_enabled = value & 0x80;
		    return;
		}
		if (addr >= 0xF4 && addr <= 0xF7) {
			spc_to_cpu_ports[addr - 0xF4] = value;
			return;
		}
		bus->write(addr, value);
	}

	void tick_timer(int index, int divider) {
		SPCTimer& timer = timers[index];

		if (!timer.enabled) {
			return;
		}

		timer.divider_counter++;

		if (timer.divider_counter < divider) {
			return;
		}

		timer.divider_counter = 0;
		timer.internal_counter++;

		uint16_t target;
		if (timer.target == 0) {
			target = 256;
		} else {
			target = timer.target;
		}

		if (timer.internal_counter >= target) {
			timer.internal_counter = 0;

			timer.output = (timer.output + 1) & 0x0F;
		}
	}

	void apply_invariants() override;

	void poll_interrupts() override;

	// Not all correct, check later when I am implementing the APU!
	bool get_flag_N() override { return (regs.P >> 7) & 0b1; }
	bool get_flag_V() override { return (regs.P >> 6) & 0b1; }
	bool get_flag_P() override { return (regs.P >> 5) & 0b1; }
 	bool get_flag_X() override { return (regs.P >> 4) & 0b1; }
	bool get_flag_H() override { return (regs.P >> 3) & 0b1; }
	bool get_flag_I() override { return (regs.P >> 2) & 0b1; } 
	bool get_flag_Z() override { return (regs.P >> 1) & 0b1;  }
	bool get_flag_C() override { return  regs.P       & 0b1;  }

	void set_flag_N(Byte value) override {
		condition = ( ( (value >> 7) & 0b1 ) == 1);
		regs.P = condition ? set_bit(regs.P, 7) : clear_bit(regs.P, 7); 
	}

	void set_flag_V(Byte value) override { return; }
	void set_flag_P(Byte value) override { return; }
	void set_flag_X(Byte value) override { return; }
	void set_flag_H(Byte value) override { return; }
	void set_flag_I(Byte value) override { return; }

	void set_flag_Z(Word value) override {
		condition = (value == 0);
		regs.P = condition ? set_bit(regs.P, 1) : clear_bit(regs.P, 1); 
	}

	void set_flag_C(Byte value) override { return; }

	void set_flag_N() override { regs.P = set_bit(regs.P, 7); }
	void set_flag_V() override { regs.P = set_bit(regs.P, 6); }
	void set_flag_P() override { regs.P = set_bit(regs.P, 5); }
	void set_flag_X() override { regs.P = set_bit(regs.P, 4); }
	void set_flag_H() override { regs.P = set_bit(regs.P, 3); }
	void set_flag_I() override { regs.P = set_bit(regs.P, 2); }
	void set_flag_Z() override { regs.P = set_bit(regs.P, 1); }
	void set_flag_C() override { regs.P = set_bit(regs.P, 0); }

	void clear_flag_N() override { regs.P = clear_bit(regs.P, 7); }
 	void clear_flag_V() override { regs.P = clear_bit(regs.P, 6); }
	void clear_flag_P() override { regs.P = clear_bit(regs.P, 5); }
	void clear_flag_X() override { regs.P = clear_bit(regs.P, 4); }
	void clear_flag_H() override { regs.P = clear_bit(regs.P, 3); }
	void clear_flag_I() override { regs.P = clear_bit(regs.P, 2); }
	void clear_flag_Z() override { regs.P = clear_bit(regs.P, 1); }
	void clear_flag_C() override { regs.P = clear_bit(regs.P, 0); }

	// Unused flags (SPC700 only)
	bool get_flag_M() override { return false; }
	bool get_flag_D() override { return false; }
	bool get_flag_B() override { return false; }

	void set_flag_M(Byte value) override { return; }
	void set_flag_D(Byte value) override { return; }
	void set_flag_B(Byte value) override { return; }

	void set_flag_M() override { return; }
	void set_flag_D() override { return; }
	void set_flag_B() override { return; }

	void clear_flag_M() override { return; }
	void clear_flag_D() override { return; }
	void clear_flag_B() override { return; }



	void enable_test_mode() override {
		return;
	}

	void disable_test_mode() override {
		return;
	}

	void reset_test_memory() override {
		return;
	}

	Byte test_peek(Address addr) override {
		return bus->read(addr);
	}

	void test_poke(Address addr, Byte value) override {
		bus->write(addr, value);
	}

private:
	std::unique_ptr<APUBus> bus;

	Byte cpu_to_spc_ports[4] {};
	Byte spc_to_cpu_ports[4] {};

	CycleCount cycle; 
	CycleCount instruction_cycle; 
	TickCount tick;

	double accumulated_cycles = 0;

	bool ipl_rom_enabled = true;

	std::array<Byte, 64> ipl_rom = {
	    0xcd, 0xef, 0xbd, 0xe8, 0x00, 0xc6, 0x1d, 0xd0,
	    0xfc, 0x8f, 0xaa, 0xf4, 0x8f, 0xbb, 0xf5, 0x78,
	    0xcc, 0xf4, 0xd0, 0xfb, 0x2f, 0x19, 0xeb, 0xf4,
	    0xd0, 0xfc, 0x7e, 0xf4, 0xd0, 0x0b, 0xe4, 0xf5,
	    0xcb, 0xf4, 0xd7, 0x00, 0xfc, 0xd0, 0xf3, 0xab,
	    0x01, 0x10, 0xef, 0x7e, 0xf4, 0x10, 0xeb, 0xba,
	    0xf6, 0xda, 0x00, 0xba, 0xf4, 0xc4, 0xf4, 0xdd,
	    0x5d, 0xd0, 0xdb, 0x1f, 0x00, 0x00, 0xc0, 0xff
	};

	SPCTimer timers[3];
};