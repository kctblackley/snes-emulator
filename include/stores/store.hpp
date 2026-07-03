#pragma once
#include "common.hpp"

class Store {
public:
	virtual ~Store() = default;

	virtual Byte read(SNESAddress address) = 0;
	virtual void write(SNESAddress address, Byte value) = 0;

	virtual SNESAddress get_address_bus() = 0;

	virtual CycleCount penalty() = 0;
	// Every store must contain a SNESAddress address_bus attribute, which this penalty is based on (if needed!)

	virtual bool is_not_open_bus() const {
		return true;
	}
};