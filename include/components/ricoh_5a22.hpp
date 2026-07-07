#pragma once
#include "cpu.hpp"
#include "ricoh_5a22_native_optable.hpp"
#include "ricoh_5a22_emulation_optable.hpp"

class Ricoh5A22 : public CPU {
public:
	explicit Ricoh5A22(Bus* bus);

	void add_cycles(CycleCount cycles) override;

	void run_half_cycle();
	void tick_component() override;
	CycleCount get_cycle() override;
	TickCount get_tick() override;

	void reset() override;
	void initialise() override;

	Byte read(Address addr) override {
		return bus->read(addr);
	}

	void write(Address addr, Byte value) override {
		bus->write(addr, value);
	}

	void apply_invariants() override;

	void poll_interrupts() override;

	bool get_flag_N() override { return (regs.P >> 7) & 0b1; }
	bool get_flag_V() override { return (regs.P >> 6) & 0b1; }
	bool get_flag_M() override { return regs.emulation_mode ? 0b1 : (regs.P >> 5) & 0b1; }
 	bool get_flag_X() override { return regs.emulation_mode ? 0b1 : (regs.P >> 4) & 0b1; }
	bool get_flag_D() override { return (regs.P >> 3) & 0b1; }
	bool get_flag_I() override { return (regs.P >> 2) & 0b1; } 
	bool get_flag_Z() override { return (regs.P >> 1) & 0b1;  }
	bool get_flag_C() override { return  regs.P       & 0b1;  }

	void set_flag_N(Byte value) override {
		condition = ( ( (value >> 7) & 0b1 ) == 1);
		regs.P = condition ? set_bit(regs.P, 7) : clear_bit(regs.P, 7); 
	}

	void set_flag_V(Byte value) override { return; }
	void set_flag_M(Byte value) override { return; }
	void set_flag_X(Byte value) override { return; }
	void set_flag_D(Byte value) override { return; }
	void set_flag_I(Byte value) override { return; }

	void set_flag_Z(Word value) override {
		condition = (value == 0);
		regs.P = condition ? set_bit(regs.P, 1) : clear_bit(regs.P, 1); 
	}

	void set_flag_C(Byte value) override { return; }

	void set_flag_N() override { regs.P = set_bit(regs.P, 7); }
	void set_flag_V() override { regs.P = set_bit(regs.P, 6); }
	void set_flag_M() override { regs.P = set_bit(regs.P, 5); }
	void set_flag_X() override { regs.P = set_bit(regs.P, 4); }
	void set_flag_D() override { regs.P = set_bit(regs.P, 3); }
	void set_flag_I() override { regs.P = set_bit(regs.P, 2); }
	void set_flag_Z() override { regs.P = set_bit(regs.P, 1); }
	void set_flag_C() override { regs.P = set_bit(regs.P, 0); }

	void clear_flag_N() override { regs.P = clear_bit(regs.P, 7); }
 	void clear_flag_V() override { regs.P = clear_bit(regs.P, 6); }
	void clear_flag_M() override { regs.P = clear_bit(regs.P, 5); }
	void clear_flag_X() override { regs.P = clear_bit(regs.P, 4); }
	void clear_flag_D() override { regs.P = clear_bit(regs.P, 3); }
	void clear_flag_I() override { regs.P = clear_bit(regs.P, 2); }
	void clear_flag_Z() override { regs.P = clear_bit(regs.P, 1); }
	void clear_flag_C() override { regs.P = clear_bit(regs.P, 0); }

	// Unused flags (SPC700 only)
	bool get_flag_P() override { return false; }
	bool get_flag_H() override { return false; }
	bool get_flag_B() override { return false; }

	void set_flag_P(Byte value) override { return; }
	void set_flag_H(Byte value) override { return; }
	void set_flag_B(Byte value) override { return; }

	void set_flag_P() override { return; }
	void set_flag_H() override { return; }
	void set_flag_B() override { return; }

	void clear_flag_P() override { return; }
	void clear_flag_H() override { return; }
	void clear_flag_B() override { return; }



	void enable_test_mode() override {
		bus->enable_test_mode();
	}

	void disable_test_mode() override {
		bus->disable_test_mode();
	}

	void reset_test_memory() override {
		bus->reset_test_memory();
	}

	Byte test_peek(Address addr) override {
		return bus->test_peek(addr);
	}

	void test_poke(Address addr, Byte value) override {
		bus->test_poke(addr, value);
	}

private:
	Bus* bus = nullptr;

	CycleCount cycle; 
	CycleCount instruction_cycle; 
	TickCount tick;
};