#include "component.hpp"
#include "ricoh_5a22.hpp"
#include "bus.hpp"

#define MDMAEN_ADDRESS 0x420b
#define HDMAEN_ADDRESS 0x420c

// This still needs to be wired up to the bus and SNES!
// Do not add to devices!

struct Pattern {
	std::array<Byte, 4> offsets;
	Byte length;
};

constexpr std::array<Pattern, 8> transfer_patterns = {{
	{{0, 0, 0, 0}, 1},
	{{0, 1, 0, 0}, 2},
	{{0, 0, 0, 0}, 2},
	{{0, 0, 1, 1}, 4},
	{{0, 1, 2, 3}, 4},
	{{0, 1, 0, 1}, 4},
	{{0, 0, 0, 0}, 2},
	{{0, 0, 1, 1}, 4},
}};

struct HDMAState {
	Word table_address = 0;
	Word indirect_address = 0;
	
	bool repeat = false;
	bool terminated = false;
	bool first_line = false;
};

enum Registers {
	DMAP = 0,
	BBAD = 1,
	A1TL = 2,
	A1TH = 3,
	A1B  = 4,
	DASL = 5,
	DASH = 6,
	DASB = 7,
	A2AL = 8,
	A2AH = 9,
	NLTR = 10
};

class DMAChannel {

public:

	Byte dmap() { return registers[Registers::DMAP]; }
	Byte bbad() { return registers[Registers::BBAD]; }
	Byte a1tl() { return registers[Registers::A1TL]; }
	Byte a1th() { return registers[Registers::A1TH]; }
	Byte  a1b() { return registers[Registers::A1B]; }
	Byte dasl() { return registers[Registers::DASL]; }
	Byte dash() { return registers[Registers::DASH]; }
	Byte dasb() { return registers[Registers::DASB]; }
	Byte a2al() { return registers[Registers::A2AL]; }
	Byte a2ah() { return registers[Registers::A2AH]; }
	Byte nltr() { return registers[Registers::NLTR]; }

	bool direction() { return (dmap() >> 7) & 0b1; }
	bool indirect_hdma() { return (dmap() >> 6) & 0b1; }
	bool fixed_transfer() { return (dmap() >> 3) & 0b1; }
	bool decrement() { return (dmap() >> 4) & 0b1; }
	Byte transfer_pattern() { return dmap() & 7; }
	Byte scanline_counter() { return nltr() & 0x7F; }
	bool reload_flag() { return (nltr() >> 7) & 0b1; }

	Word a1t() { return (a1th() << 8) | a1tl(); }
	Word das() { return (dash() << 8) | dasl(); }
	Word a2a() { return (a2ah() << 8) | a2al(); }

	void set_a1t(Word word) {
		registers[A1TL] = get_lo(word);
		registers[A1TH] = get_hi(word);
	}

	void set_das(Word word) {
		registers[DASL] = get_lo(word);
		registers[DASH] = get_hi(word);
	}

	void set_a2a(Word word) {
		registers[A2AL] = get_lo(word);
		registers[A2AH] = get_hi(word);
	}

	std::array<Byte, 16> registers {};
	HDMAState hdma;
};

class DMAController : public Component {
public:
	void reset() override {
		mdmaen = 0x00;
		hdmaen = 0x00;
	}

	void initialise() override {
		return;
	}

	Byte read(Address addr) override {
		return 0xFF;
	};

	void write(Address addr, Byte value) override {
		return;
	};

	void execute_gpdma();
	void execute_hdma();
	void hdma_init();
	CycleCount perform_hdma_transfer(DMAChannel& ch);
	CycleCount load_hdma_descriptor(DMAChannel& ch);

	Byte communication_read(SNESAddress addr) override {
		if (addr.offset == MDMAEN_ADDRESS) {
			return mdmaen;
		}
		if (addr.offset == HDMAEN_ADDRESS) {
			return hdmaen;
		}
		Byte channel_number = (addr.offset >> 4) & 0x07;
		Byte register_number = addr.offset & 0x0F;
		return channels[channel_number].registers[register_number];
	}

	void communication_write(SNESAddress addr, Byte value) override {
		// MDMAEN and HDMAEN to be added!
		if (addr.offset == MDMAEN_ADDRESS) { // GPDMA implementation goes here
			mdmaen = value;
			execute_gpdma();
			//std::cout << "CALL GPDMA\n" << (int)value;
			mdmaen = 0;
			return;
		}
		if (addr.offset == HDMAEN_ADDRESS) { // HDMA implementation goes here
			hdmaen = value;
			// Games write to this to specify which channels should retrigger on every HBlank!
			//std::cout << "CALL HDMA\n";
			return;
		}
		// Also add guards
		Byte channel_number = (addr.offset >> 4) & 0x07;
		Byte register_number = addr.offset & 0x0F;
		channels[channel_number].registers[register_number] = value;
	}

	void connect_cpu(Ricoh5A22* cpu) {
		this->cpu = cpu;
	}

	void connect_bus(Bus* bus) {
		this->bus = bus;
	}

	// Unused (overrided to allow DMAController to be a Component)
	void add_cycles(CycleCount cycles) override {
		return;
	}

	void tick_component() override {
		return;
	}

	CycleCount get_cycle() override {
		return 0;
	}

	TickCount get_tick() override {
		return 0;
	}

private:
	Byte mdmaen = 0x00;
	Byte hdmaen = 0x00;

	DMAChannel channels[8];

	Ricoh5A22* cpu = nullptr;
	Bus* bus = nullptr;
};