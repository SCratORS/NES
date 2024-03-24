#pragma once
#include <cstdint>
#include "RP6502.h"
#include "RP2C02.h"
#include "RP2A03.h"
#include "CARTRIDGE.h"

class BUS
{
public:
	BUS();
	~BUS();
	
	RP2A03 APU;
	RP6502 CPU;
	RP2C02 PPU;
	CARTRIDGE * CART;

	uint8_t RAM[0x800];

	uint8_t MemAccess(uint16_t addr, uint8_t data = 0x00, bool write = false);

	void Clock();
	void Reset();
	void ConnectCartridge(CARTRIDGE * cartridge);

private:
	uint32_t nSystemClockCounter = 0;
};