#pragma once
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

using Byte = uint8_t;
using Bank = uint8_t;
using Quadrant = uint8_t;
using Offset = uint16_t;
using Address = uint32_t;
using CycleCount = uint64_t;
using TickCount = uint64_t;

typedef struct {
	Bank bank;
	Offset offset;
} SNESAddress;

SNESAddress split_address(Address address);

Quadrant get_quadrant(Bank bank);
// Timing constants
constexpr CycleCount MASTER_CLOCK = 21477272;
constexpr CycleCount RICOH_5A22_HALF_CYCLE = 3;

// WRAM constants and WRAM Access Constants
constexpr size_t WRAM_SIZE = 128 * 1024;
constexpr size_t WRAM_BANK_SIZE = 64 * 1024;
constexpr CycleCount WRAM_PENALTY = 8;

// System Area Constants (where each section of system area quadrants begin)
constexpr Offset WRAM_SECTION = 0x0000;
constexpr Offset OPEN_BUS_SECTION = 0x2000;
constexpr Offset PPU_PORTS_SECTION = 0x2100;
constexpr Offset APU_PORTS_SECTION = 0x2140;
constexpr Offset WRAM_ACCESS_SECTION = 0x2180;
constexpr Offset CPU_PORTS_SECTION = 0x4000;
constexpr Offset CPU_DMA_PORTS_SECTION = 0x4300;
constexpr Offset EXPANSION_DATA_SECTION = 0x6000;
constexpr Offset CARTRIDGE_SECTION = 0x8000;
constexpr Offset MAX_OFFSET_SECTION = 0xFFFF;

constexpr size_t PPU_PORTS_SIZE = APU_PORTS_SECTION - PPU_PORTS_SECTION;
constexpr size_t APU_PORTS_SIZE = WRAM_ACCESS_SECTION - APU_PORTS_SECTION;
constexpr size_t WRAM_ACCESS_SIZE = CPU_PORTS_SECTION - WRAM_ACCESS_SECTION;
constexpr size_t CPU_PORTS_SIZE = CPU_DMA_PORTS_SECTION - CPU_PORTS_SECTION;
constexpr size_t CPU_DMA_PORTS_SIZE = EXPANSION_DATA_SECTION - CPU_DMA_PORTS_SECTION;
constexpr size_t EXPANSION_DATA_SIZE = CARTRIDGE_SECTION - EXPANSION_DATA_SECTION;
