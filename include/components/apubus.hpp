class APUBus {
public:
	APUBus() {}

	// To change later!
	void enable_test_mode() { return; }
	void disable_test_mode() { return; }
	void reset_test_memory() { return; }
	Byte test_peek(Address addr) { return 0xFF; }
	void test_poke(Address addr, Byte value) { return; }
};