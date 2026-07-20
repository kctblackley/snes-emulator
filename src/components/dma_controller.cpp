#include "dma_controller.hpp"

// GPDMA

void DMAController::execute_gpdma() {
	CycleCount total_cycles = 8;
	if (mdmaen != 0) {
		for (int channel = 0; channel < 8; channel++) {
			if (((mdmaen >> channel) & 0b1) == 1) {
				total_cycles += 8;
				DMAChannel& ch = channels[channel];
				
				Word a_address = ch.a1t();
				Byte a_bank = ch.a1b();

				Word b_address = 0x2100 + ch.bbad();
				uint32_t count = ch.das();

				if (count == 0) {
					count = 0x10000;
				}

				const Pattern& pattern = transfer_patterns[ch.transfer_pattern()];
				int step = 0;

				while (count > 0) {
					Byte offset = pattern.offsets[step % pattern.length];
					Address port = b_address + offset;
					Address addr = (a_bank << 16) | a_address;
					if (ch.direction() == 0) {
						Byte byte = bus->read(addr, true);
						bus->write(port, byte, true);
					} else {
						Byte byte = bus->read(port, true);
						bus->write(addr, byte, true);
					}

					if (!ch.fixed_transfer()) {
						a_address += ch.decrement() ? -1 : 1;
						a_address = a_address & 0xFFFF;
					}

					count -= 1;
					step += 1;
					total_cycles += 8;
				}

				ch.set_a1t(a_address);
				ch.set_das(count);
			}
		}

		//std::cout << "Transfer completed" << std::endl;
		if (cpu) {
			cpu->add_cycles(total_cycles);
		}
	}
}

// HDMA

void DMAController::hdma_init() {
    CycleCount cycles = 0;

    for (int channel = 0; channel < 8; channel++) {
        if (((hdmaen >> channel) & 1) == 0) {
            continue;
        }

        DMAChannel& ch = channels[channel];

        ch.hdma.terminated = false;
        ch.hdma.first_line = true;

        Word table = ch.a1t();

        Byte descriptor =
            bus->read((ch.a1b() << 16) | table, true);

        table++;
        cycles += 8;

        if (descriptor == 0) {
            ch.hdma.terminated = true;
            continue;
        }

        ch.registers[NLTR] = descriptor;

        if (ch.indirect_hdma()) {

            Byte lo =
                bus->read((ch.a1b() << 16) | table, true);
            table++;
            cycles += 8;

            Byte hi =
                bus->read((ch.a1b() << 16) | table, true);
            table++;
            cycles += 8;

            Word indirect = lo | (hi << 8);

            ch.hdma.indirect_address = indirect;
            ch.set_das(indirect);
        }

        ch.hdma.table_address = table;
        ch.set_a2a(table);
    }

    if (cpu) {
        cpu->add_cycles(cycles);
    }
}

CycleCount DMAController::perform_hdma_transfer(DMAChannel& ch) {
    CycleCount cycles = 0;

    const Pattern& pattern =
        transfer_patterns[ch.transfer_pattern()];

    Word source =
        ch.indirect_hdma()
            ? ch.das()
            : ch.a2a();

    Byte bank =
        ch.indirect_hdma()
            ? ch.dasb()
            : ch.a1b();

    Word bbase = 0x2100 + ch.bbad();

    for (int i = 0; i < pattern.length; i++) {

        Byte offset = pattern.offsets[i];

        Address source_address =
            (bank << 16) | source;

        Address dest_address =
            bbase + offset;

        Byte value =
            bus->read(source_address, true);

        bus->write(dest_address, value, true);

        source = (source + 1) & 0xFFFF;

        cycles += 8;
    }

    if (ch.indirect_hdma()) {
        //std::cout << "PERFORMED INDIRECT HDMA\n";
        ch.set_das(source);
        ch.hdma.indirect_address = source;
    }
    else {
    	//std::cout << "PERFORMED COMMON HDMA\n";
        ch.set_a2a(source);
        ch.hdma.table_address = source;
    }

    return cycles;
}

void DMAController::execute_hdma() {
    CycleCount cycles = 0;

    for (int channel = 0; channel < 8; channel++) {

        if (((hdmaen >> channel) & 1) == 0) {
            continue;
        }

        DMAChannel& ch = channels[channel];

        if (ch.hdma.terminated) {
            continue;
        }

        cycles += 8;

        Byte nltr = ch.nltr();

        bool repeat =
            (nltr & 0x80) != 0;

        Byte count =
            nltr & 0x7F;

        if (count == 0) {
            continue;
        }

        bool perform_transfer =
            ch.hdma.first_line || repeat;

        if (perform_transfer) {
            cycles += perform_hdma_transfer(ch);
        }

        ch.hdma.first_line = false;

        count--;

        if (count == 0) {
            cycles += load_hdma_descriptor(ch);
        }
        else {
            ch.registers[NLTR] =
                (repeat ? 0x80 : 0x00) | count;
        }
    }

    if (cpu) {
        cpu->add_cycles(cycles);
    }
}

CycleCount DMAController::load_hdma_descriptor(DMAChannel& ch) {
    CycleCount cycles = 0;

    Word table = ch.hdma.table_address;

    Byte descriptor =
        bus->read((ch.a1b() << 16) | table, true);

    table++;
    cycles += 8;

    if (descriptor == 0) {
        ch.hdma.terminated = true;
        return cycles;
    }

    ch.registers[NLTR] = descriptor;

    if (ch.indirect_hdma()) {

        Byte lo =
            bus->read((ch.a1b() << 16) | table, true);
        table++;
        cycles += 8;

        Byte hi =
            bus->read((ch.a1b() << 16) | table, true);
        table++;
        cycles += 8;

        Word indirect = lo | (hi << 8);

        ch.hdma.indirect_address = indirect;
        ch.set_das(indirect);
    }

    ch.hdma.table_address = table;
    ch.set_a2a(table);

    ch.hdma.first_line = true;

    return cycles;
}