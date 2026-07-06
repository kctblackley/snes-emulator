#include "test_harness.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "options.hpp"
#include "json.hpp"
#include "opcode_engine.hpp"
#include "ricoh_5a22.hpp"
#include "ricoh_5a22_optable.hpp"

namespace {

constexpr const char* COLOR_GREEN  = "\033[32m";
constexpr const char* COLOR_RED    = "\033[31m";
constexpr const char* COLOR_YELLOW = "\033[33m";
constexpr const char* COLOR_BOLD   = "\033[1m";
constexpr const char* COLOR_RESET  = "\033[0m";

struct RegisterState {
	Word pc = 0;
	Word s = 0;
	Byte p = 0;
	Word a = 0;
	Word x = 0;
	Word y = 0;
	Byte dbr = 0;
	Word d = 0;
	Byte pbr = 0;
	bool e = false;
};

struct MemoryEntry {
	Address address = 0;
	Byte value = 0;
};

struct TestCase {
	std::string name;
	RegisterState initial;
	RegisterState final_state;
	std::vector<MemoryEntry> initial_ram;
	std::vector<MemoryEntry> final_ram;
};

struct Mismatch {
	std::string field;
	std::string expected;
	std::string actual;
};

RegisterState parse_register_state(const JsonValue& node) {
	RegisterState state;
	state.pc  = static_cast<Word>(node["pc"].as_int());
	state.s   = static_cast<Word>(node["s"].as_int());
	state.p   = static_cast<Byte>(node["p"].as_int());
	state.a   = static_cast<Word>(node["a"].as_int());
	state.x   = static_cast<Word>(node["x"].as_int());
	state.y   = static_cast<Word>(node["y"].as_int());
	state.dbr = static_cast<Byte>(node["dbr"].as_int());
	state.d   = static_cast<Word>(node["d"].as_int());
	state.pbr = static_cast<Byte>(node["pbr"].as_int());
	state.e   = node["e"].as_int() != 0;
	return state;
}

std::vector<MemoryEntry> parse_ram(const JsonValue& state_node) {
	std::vector<MemoryEntry> entries;
	const JsonValue& ram = state_node["ram"];
	entries.reserve(ram.size());

	for (size_t i = 0; i < ram.size(); i++) {
		const JsonValue& pair = ram[i];
		MemoryEntry entry;
		entry.address = static_cast<Address>(pair[0].as_int());
		entry.value   = static_cast<Byte>(pair[1].as_int());
		entries.push_back(entry);
	}
	return entries;
}

TestCase parse_test_case(const JsonValue& node) {
	TestCase test_case;
	test_case.name         = node["name"].as_string();
	test_case.initial      = parse_register_state(node["initial"]);
	test_case.final_state  = parse_register_state(node["final"]);
	test_case.initial_ram  = parse_ram(node["initial"]);
	test_case.final_ram    = parse_ram(node["final"]);
	return test_case;
}

void apply_initial_state(Ricoh5A22& cpu, const TestCase& test_case) {
	cpu.regs.PC = test_case.initial.pc;
	cpu.regs.S  = test_case.initial.s;
	cpu.regs.P  = test_case.initial.p;
	cpu.regs.A  = test_case.initial.a;
	cpu.regs.X  = test_case.initial.x;
	cpu.regs.Y  = test_case.initial.y;
	cpu.regs.DB = test_case.initial.dbr;
	cpu.regs.D  = test_case.initial.d;
	cpu.regs.PB = test_case.initial.pbr;
	cpu.regs.emulation_mode = test_case.initial.e;

	for (const MemoryEntry& entry : test_case.initial_ram) {
		cpu.test_poke(entry.address, entry.value);
	}
}

void run_instruction(Ricoh5A22& cpu, Byte opcode_value) {
	CycleCount idx = 0;
	constexpr CycleCount MAX_STEPS = 64; // guards against a runaway/incomplete instruction
	CycleCount steps = 0;

	do {
		Opcode op = get_opcode(optable, opcode_value, idx, cpu);
		op.function(cpu, op.skipped);
		steps++;
	} while (idx != 0 && steps < MAX_STEPS);
}

std::string to_hex(unsigned long long value, int width) {
	std::ostringstream oss;
	oss << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(width) << value;
	return oss.str();
}

void compare_registers(const RegisterState& expected, Ricoh5A22& cpu, std::vector<Mismatch>& mismatches) {
	auto check = [&](const std::string& field, unsigned long long exp, unsigned long long act, int width) {
		if (exp != act) {
			mismatches.push_back({field, to_hex(exp, width), to_hex(act, width)});
		}
	};

	check("PC",  expected.pc,  cpu.regs.PC, 4);
	check("S",   expected.s,   cpu.regs.S,  4);
	check("P",   expected.p,   cpu.regs.P,  2);
	check("A",   expected.a,   cpu.regs.A,  4);
	check("X",   expected.x,   cpu.regs.X,  4);
	check("Y",   expected.y,   cpu.regs.Y,  4);
	check("DBR", expected.dbr, cpu.regs.DB, 2);
	check("D",   expected.d,   cpu.regs.D,  4);
	check("PBR", expected.pbr, cpu.regs.PB, 2);
	check("E",   expected.e ? 1u : 0u, cpu.regs.emulation_mode ? 1u : 0u, 1);
}

void compare_memory(const std::vector<MemoryEntry>& expected, Ricoh5A22& cpu, std::vector<Mismatch>& mismatches) {
	for (const MemoryEntry& entry : expected) {
		Byte actual = cpu.test_peek(entry.address);
		if (actual != entry.value) {
			std::ostringstream field;
			field << "RAM[" << to_hex(entry.address, 6) << "]";
			mismatches.push_back({field.str(), to_hex(entry.value, 2), to_hex(actual, 2)});
		}
	}
}

}

bool test(const std::string& opcode_name, Byte opcode_value) {
	const std::string path = "tests/" + opcode_name + ".json";

	JsonValue root;
	try {
		root = parse_json_file(path);
	} catch (const std::exception& e) {
		std::cerr << COLOR_RED << "Could not load test file '" << path << "': "
		          << e.what() << COLOR_RESET << std::endl;
		return false;
	}

	if (root.type != JsonValue::Type::Array) {
		std::cerr << COLOR_RED << "Test file '" << path << "' does not contain a JSON array."
		          << COLOR_RESET << std::endl;
		return false;
	}

	Bus bus;
	bus.set_wait_callback([](CycleCount) {}); // test mode never touches this, but keep it well-defined
	Ricoh5A22 cpu(&bus);
	cpu.enable_test_mode();

	size_t passed = 0;
	size_t failed = 0;
	std::vector<std::string> failed_names;

	/*std::cout << COLOR_BOLD << "\n===== Testing opcode \"" << opcode_name << "\" ("
	          << to_hex(opcode_value, 2) << ") - " << root.size() << " test case(s) =====\n"
	          << COLOR_RESET << std::endl;*/

	for (size_t i = 0; i < root.size(); i++) {
		TestCase test_case = parse_test_case(root[i]);

		cpu.reset_test_memory();
		apply_initial_state(cpu, test_case);

		run_instruction(cpu, opcode_value);

		std::vector<Mismatch> mismatches;
		compare_registers(test_case.final_state, cpu, mismatches);
		compare_memory(test_case.final_ram, cpu, mismatches);

		if (mismatches.empty()) {
			passed++;
			//std::cout << COLOR_GREEN << "[PASSED] " << COLOR_RESET << test_case.name << "\n";
		} else {
			failed++;
			failed_names.push_back(test_case.name);
			
			if constexpr(!SEE_TOTAL_PASS_ONLY) {
				std::cout << COLOR_RED << "[FAILED] " << COLOR_RESET << test_case.name << "\n";
				for (const Mismatch& m : mismatches) {
					std::cout << "         " << COLOR_YELLOW << m.field << COLOR_RESET
					          << " expected " << m.expected << " but got " << m.actual << "\n";
				}
			}
		}
	}

	//std::cout << "\n" << COLOR_BOLD << "----- Summary for " << opcode_name << " -----" << COLOR_RESET << "\n";
	if (failed > 0) {
		std::cout << COLOR_BOLD << opcode_name << " " << COLOR_GREEN << passed << " passed" << COLOR_RESET << ", "
	    	<< (failed > 0 ? COLOR_RED : COLOR_GREEN) << failed << " failed" << COLOR_RESET
	        << " out of " << root.size() << " total" << std::endl;
	}
	if constexpr(!SEE_TOTAL_PASS_ONLY) {
		if (failed > 0) {
			std::cout << COLOR_RED << "\nFailed tests:" << COLOR_RESET << "\n";
			for (const std::string& name : failed_names) {
				std::cout << "  - " << name << "\n";
			}
			std::cout << std::endl;
		}
	}

	return (failed == 0);
}
