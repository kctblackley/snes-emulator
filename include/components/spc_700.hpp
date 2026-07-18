#pragma once
#include "cpu.hpp"
#include "common.hpp"
#include "apubus.hpp"

class SPC700 : public CPU {
public:
	SPC700();

	void add_cycles(CycleCount cycles) override;

	void run_half_cycle();
	void tick_component() override;
	CycleCount get_cycle() override;
	TickCount get_tick() override;

	void reset() override;
	void initialise() override;

	// STUB I GOT ONLINE! WILL NEED TO REPLACE WHEN I WORK ON THIS MYSELF!!!
	// For communication from main CPU bus
	//
	// This stub fakes the CPU<->SPC700 handshake without running real SPC700
	// code. Two mechanisms:
	//  1) Immediate echo: whatever the CPU writes to a port becomes readable
	//     on that same port right away. This satisfies "wait until $2140
	//     equals what I just wrote" loops, which cover most of the standard
	//     IPL upload protocol.
	//  2) Stall recovery: real games commonly upload a tiny bootstrap loader
	//     to SPC RAM, jump to it, and then wait for *that* code to announce
	//     readiness the same way the original IPL ROM did (by putting $AA on
	//     port 0 and $BB on port 1) before starting a second transfer stage.
	//     Since no real SPC700 code ever actually runs here, that
	//     announcement never happens on its own, and the CPU spins forever.
	//     We detect "reading the same unchanged value over and over with no
	//     new writes" as a stall and re-arm ports 0/1 back to $AA/$BB, as if
	//     a loader had just come up and announced it's ready. This unblocks
	//     the CPU without needing real SPC700 execution -- games proceed with
	//     no audio instead of hanging.
	Byte communication_read(SNESAddress addr) override {
		Byte port = addr.offset & 0x3;
		if (port == 0 || port == 1) {
			handshake_active = true;

			if (apu_to_cpu_ports[port] == last_polled_value[port]) {
				stall_counter++;
			} else {
				stall_counter = 0;
			}
			last_polled_value[port] = apu_to_cpu_ports[port];

			if (stall_counter > STALL_THRESHOLD) {
				apu_to_cpu_ports[0] = 0xAA;
				apu_to_cpu_ports[1] = 0xBB;
				stall_counter = 0;
			}
		}
		//std::cout << "Reading from APU to CPU: Port " << (int)(port) << "\n";
		//std::cout << "Value is: " << (int)(apu_to_cpu_ports[port]) << "\n";
		return apu_to_cpu_ports[port];
	}

	void communication_write(SNESAddress addr, Byte value) override {
		Byte port = addr.offset & 0x3;
		stall_counter = 0; // any write counts as forward progress
		if (handshake_active) {
			apu_to_cpu_ports[port] = value;
		}
		//std::cout << "Writing from CPU to APU: Port " << (int)(port) << "\n";
		//std::cout << "Value is " << (int)value << "\n";
		cpu_to_apu_ports[port] = value;
	}

	// Stubbed until I implement the SPC700 in full!
	Byte read(Address addr) override {
		return 0xFF;
	}

	void write(Address addr, Byte value) override {
		return;
	}

	void apply_invariants() override;

	void poll_interrupts() override;

	// Not all correct, check later when I am implementing the APU!
	bool get_flag_N() override { return (regs.P >> 7) & 0b1; }
	bool get_flag_V() override { return (regs.P >> 6) & 0b1; }
	bool get_flag_P() override { return (regs.P >> 5) & 0b1; }
 	bool get_flag_B() override { return (regs.P >> 4) & 0b1; }
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
	void set_flag_B(Byte value) override { return; }
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
	void set_flag_B() override { regs.P = set_bit(regs.P, 4); }
	void set_flag_H() override { regs.P = set_bit(regs.P, 3); }
	void set_flag_I() override { regs.P = set_bit(regs.P, 2); }
	void set_flag_Z() override { regs.P = set_bit(regs.P, 1); }
	void set_flag_C() override { regs.P = set_bit(regs.P, 0); }

	void clear_flag_N() override { regs.P = clear_bit(regs.P, 7); }
 	void clear_flag_V() override { regs.P = clear_bit(regs.P, 6); }
	void clear_flag_P() override { regs.P = clear_bit(regs.P, 5); }
	void clear_flag_B() override { regs.P = clear_bit(regs.P, 4); }
	void clear_flag_H() override { regs.P = clear_bit(regs.P, 3); }
	void clear_flag_I() override { regs.P = clear_bit(regs.P, 2); }
	void clear_flag_Z() override { regs.P = clear_bit(regs.P, 1); }
	void clear_flag_C() override { regs.P = clear_bit(regs.P, 0); }

	// Unused flags (SPC700 only)
	bool get_flag_M() override { return false; }
	bool get_flag_D() override { return false; }
	bool get_flag_X() override { return false; }

	void set_flag_M(Byte value) override { return; }
	void set_flag_D(Byte value) override { return; }
	void set_flag_X(Byte value) override { return; }

	void set_flag_M() override { return; }
	void set_flag_D() override { return; }
	void set_flag_X() override { return; }

	void clear_flag_M() override { return; }
	void clear_flag_D() override { return; }
	void clear_flag_X() override { return; }



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

	APUBus* bus = nullptr;

	Byte cpu_to_apu_ports[4] = {0x00, 0x00, 0x00, 0x00};
	Byte apu_to_cpu_ports[4] = {0xAA, 0xBB, 0x00, 0x00};

	CycleCount cycle; 
	CycleCount instruction_cycle; 
	TickCount tick;

	bool handshake_active = false;

	// Stall recovery state (see communication_read)
	static constexpr int STALL_THRESHOLD = 500;
	int stall_counter = 0;
	Byte last_polled_value[4] = {0xFF, 0xFF, 0xFF, 0xFF};
};

