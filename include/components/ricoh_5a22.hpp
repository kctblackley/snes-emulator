#pragma once
#include "cpu.hpp"
#include "ricoh_5a22_native_optable.hpp"
#include "ricoh_5a22_emulation_optable.hpp"
#include "renderer.hpp"
#include "ppu.hpp"

// Multiplier addresses
#define WRMPYA_ADDRESS 0x4202
#define WRMPYB_ADDRESS 0x4203
#define RDMPYL_ADDRESS 0x4216
#define RDMPYH_ADDRESS 0x4217

#define MULTIPLIER_HALF_CYCLES 16

// Division addresses
#define WRDIVL_ADDRESS 0x4204
#define WRDIVH_ADDRESS 0x4205
#define WRDIVB_ADDRESS 0x4206
#define RDDIVL_ADDRESS 0x4214
#define RDDIVH_ADDRESS 0x4215

#define DIVISOR_HALF_CYCLES 32

// CPU register addresses
#define HVBJOY_ADDRESS 0x4212
#define JOY1L_ADDRESS 0x4218
#define JOY1H_ADDRESS 0x4219

// Interrupts
#define OPCODE_NMI 0x100
#define OPCODE_IRQ 0x101
#define NMITIMEN_ADDRESS 0x4200
#define HTIMEL_ADDRESS 0x4207
#define HTIMEH_ADDRESS 0x4208
#define VTIMEL_ADDRESS 0x4209
#define VTIMEH_ADDRESS 0x420A
#define RDNMI_ADDRESS 0x4210
#define TIMEUP_ADDRESS 0x4211

/*
To do:
1. Create all handling of interrupt registers
2. Create mechanism to send all relevant information to PPU
3. Create mechanism that signals PPU interrupts via writes
4. Then in communication_write of CPU, handle interrupts
*/

// The Multiplier's RDMPYH and RDMPYL also store the remainder of division by the divisor
// Multiplication takes 16 half-cycles of the CPU (not master clock)
struct Multiplier {
	Byte WRMPYA, WRMPYB; // first and second number to multiply, writing to WRDMPYB starts multiplication
	Byte RDMPYH, RDMPYL; // read result of multiplication
	CycleCount half_cycles_since_init = 0; // Only when this is 0 can 
	bool completed = true;
};

// Division takes 32 half-cycles of the CPU (not master clock)
struct Divisor {
	Byte WRDIVH, WRDIVL; // dividend
	Byte WRDIVB; // divisor (starts division)
	Byte RDDIVH, RDDIVL; // read result of division
	CycleCount half_cycles_since_init = 0;
	bool completed = true;
};

// Adding interrupt registers
struct CPURegisters {
	Byte HVBJOY;
	Byte NMITIMEN;
	Byte HTIMEL, HTIMEH;
	Byte VTIMEL, VTIMEH;
	Byte RDNMI, TIMEUP;
};

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

	// Handles CPU ports!
	Byte communication_read(SNESAddress addr) override {
		// Multiplier
		if (multiplier.half_cycles_since_init == 0) {
			// Carry out multiplication here
			if (!multiplier.completed) {
				uint16_t result = multiplier.WRMPYA * multiplier.WRMPYB;
				multiplier.RDMPYL = get_lo(result);
				multiplier.RDMPYH = get_hi(result);
				multiplier.completed = true;
			}

			if (addr.offset == RDMPYL_ADDRESS) { return multiplier.RDMPYL; }
			if (addr.offset == RDMPYH_ADDRESS) { return multiplier.RDMPYH; }

		}
		// Divisor

		if (divisor.half_cycles_since_init == 0) {

			// Carry out division here
			if (!divisor.completed) {
				uint16_t dividend_value = (divisor.WRDIVH << 8) | divisor.WRDIVL;
				uint8_t divisor_value = divisor.WRDIVB;
				uint16_t result = 0xFFFF;
				uint16_t remainder = 0xFFFF;
				if (divisor_value != 0) {
					result = (unsigned int)(dividend_value) / (unsigned int)(divisor_value);
					remainder = dividend_value % divisor_value;
				}

				divisor.RDDIVL = get_lo(result);
				divisor.RDDIVH = get_hi(result);

				multiplier.RDMPYL = get_lo(remainder);
				multiplier.RDMPYH = get_hi(remainder);

				divisor.completed = true;
			}

			if (addr.offset == RDDIVL_ADDRESS) { return divisor.RDDIVL; }
			if (addr.offset == RDDIVH_ADDRESS) { return divisor.RDDIVH; }

			if (addr.offset == RDMPYL_ADDRESS) { return multiplier.RDMPYL; }
			if (addr.offset == RDMPYH_ADDRESS) { return multiplier.RDMPYH; }

		}

		// Interrupt handling
		if (addr.offset == RDNMI_ADDRESS) {
			mregs.RDNMI = mregs.RDNMI & 0x7F;
			nmi_line = false;
			return mregs.RDNMI;
		}
		if (addr.offset == TIMEUP_ADDRESS) {
			mregs.TIMEUP = mregs.TIMEUP & 0x7F;
			irq_line = false;
			return mregs.TIMEUP;
		}


		if (addr.offset == HVBJOY_ADDRESS) {
			//std::cout << "READING HVBJOY AS " << std::hex << mregs.HVBJOY << "\n" ;
			return mregs.HVBJOY;
		}
		
		if (addr.offset == JOY1L_ADDRESS || addr.offset == JOY1H_ADDRESS) {
			return renderer->get_joypad(addr.offset);
		}



		return 0x00;
	}

	void communication_write(SNESAddress addr, Byte value) override {
		// Multiplier
		if (addr.offset == WRMPYA_ADDRESS) { multiplier.WRMPYA = value; }
		if (addr.offset == WRMPYB_ADDRESS) { multiplier.WRMPYB = value; }
			
		if (addr.offset == WRMPYB_ADDRESS) {
			multiplier.half_cycles_since_init = MULTIPLIER_HALF_CYCLES;
			multiplier.completed = false;
		}

		// Divisor
		if (addr.offset == WRDIVH_ADDRESS) { divisor.WRDIVH = value; }
		if (addr.offset == WRDIVL_ADDRESS) { divisor.WRDIVL = value; }
		if (addr.offset == WRDIVB_ADDRESS) { divisor.WRDIVB = value; }

		if (addr.offset == WRDIVB_ADDRESS) {
			divisor.half_cycles_since_init = DIVISOR_HALF_CYCLES;
			divisor.completed = false;
		}

		if (addr.offset == HVBJOY_ADDRESS) { mregs.HVBJOY = value; }

		// Interrupts
		if (addr.offset == NMITIMEN_ADDRESS) {
			Byte irq_bits_before = mregs.NMITIMEN & 0x30;
			nmi_enabled = (((value >> 7) & 0b1) == 1);
			auto_read_enabled = ((value & 0b1) == 1);

			this->irq_mode = (value >> 4) & 3;
			ppu->irq_mode = this->irq_mode;
			mregs.NMITIMEN = value;

			if (irq_bits_before != 0 && (value & 0x30) == 0) {
				mregs.TIMEUP = mregs.TIMEUP & ~0x80;
				irq_line = false;
			}
		}
		if (addr.offset == HTIMEL_ADDRESS) {
			mregs.HTIMEL = value;
			ppu->h_time_target = (mregs.HTIMEH << 8) | mregs.HTIMEL;
		}
		if (addr.offset == HTIMEH_ADDRESS) {
			mregs.HTIMEH = value;
			ppu->h_time_target = (mregs.HTIMEH << 8) | mregs.HTIMEL;
		}
		if (addr.offset == VTIMEL_ADDRESS) {
			mregs.VTIMEL = value;
			ppu->v_time_target = (mregs.VTIMEH << 8) | mregs.VTIMEL;
		}
		if (addr.offset == VTIMEH_ADDRESS) {
			mregs.VTIMEH = value;
			ppu->v_time_target = (mregs.VTIMEH << 8) | mregs.VTIMEL;
		}
		if (addr.offset == RDNMI_ADDRESS) {
			mregs.RDNMI = mregs.RDNMI | value;
			if (value == 0x00) {
				mregs.RDNMI = 0x00;
			}
			if (nmi_enabled && (mregs.RDNMI & 0x80)) {
				nmi_line = true;
			}
		}
		if (addr.offset == TIMEUP_ADDRESS) {
			mregs.TIMEUP = mregs.TIMEUP | value;
			if (value == 0x00) {
				mregs.TIMEUP = 0x00;
			}
			if (mregs.TIMEUP & 0x80) {
				irq_line = true;
			}
		}
	}

	void tick_multiply_divisor();

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
	void connect_renderer(Renderer* renderer) {
		this->renderer = renderer;
	}
	void connect_ppu(PPU* ppu) {
		this->ppu = ppu;
	}

	void log();


private:
	
	Bus* bus = nullptr;

	CycleCount cycle; 
	CycleCount instruction_cycle; 
	TickCount tick;

	Multiplier multiplier;
	Divisor divisor;

	CPURegisters mregs; // memory-mapped registers

	Renderer* renderer = nullptr;
	PPU* ppu = nullptr;

	Byte irq_mode;
	bool nmi_line = false;
	bool irq_line = false;
	bool nmi_enabled = false;
	bool auto_read_enabled = false;
};