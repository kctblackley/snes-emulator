#include "store.hpp"

#define WMDATA_ADDRESS 0x2180
#define WMADDL_ADDRESS 0x2181
#define WMADDM_ADDRESS 0x2182
#define WMADDH_ADDRESS 0x2183

class WRAM : public Store {
public:

	Byte read(SNESAddress address) override {
		if (address.bank != 0x7E && address.bank != 0x7F) {
			if (address.offset == WMADDL_ADDRESS || address.offset == WMADDM_ADDRESS || address.offset == WMADDH_ADDRESS) {
				return 0x00;
			}
			if (address.offset == WMDATA_ADDRESS) {
				Byte fetched = wram[wmadd];
				wmadd = (wmadd + 1) & 0x1FFFF;
				return fetched;
			}
		}
		address_bus = address;
		return wram[get_index(address)];
	}

	void write(SNESAddress address, Byte value) override {
		if (address.bank != 0x7E && address.bank != 0x7F) {
			if (address.offset == WMADDL_ADDRESS) {
				wmadd = (wmadd & 0x1FF00) | value;
				wmadd = wmadd & 0x1FFFF;
				return;
			}
			if (address.offset == WMADDM_ADDRESS) {
				wmadd = (wmadd & 0x100FF) | (value << 8);
				wmadd = wmadd & 0x1FFFF;
				return;
			}
			if (address.offset == WMADDH_ADDRESS) {
				wmadd = (wmadd & 0x0FFFF) | ((value & 1) << 16);
				wmadd = wmadd & 0x1FFFF;
				return;
			}
			if (address.offset == WMDATA_ADDRESS) {
				wram[wmadd] = value;
				wmadd = (wmadd + 1) & 0x1FFFF;
				return;
			}
		}
		address_bus = address;
		wram[get_index(address)] = value;
	}

	SNESAddress get_address_bus() override {
		return address_bus;
	}

	CycleCount penalty() override {
		return WRAM_PENALTY;
	}

private:
	std::array<Byte, WRAM_SIZE> wram {};
	SNESAddress address_bus;

	uint64_t get_index(SNESAddress address) {
		if (address.bank == 0x7E || address.bank == 0x7F) {
			return address.offset + (WRAM_BANK_SIZE * (address.bank - 0x7E) );
		} else {
			return address.offset;
		}
	}

	uint32_t wmadd = 0x00;
};