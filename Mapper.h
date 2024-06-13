#pragma once

#include <cstdint>

class Mapper {
public:
	Mapper(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper();

	virtual bool CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write = false) = 0;
	virtual bool PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write = false) = 0;
	virtual void reset() = 0;
	virtual uint8_t mirror();
	virtual bool irqState();
	virtual void irqClear();
	virtual void LoadState() {};
	virtual void SaveState() {};
	virtual void scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control);

protected:
	uint8_t nPRGBanks = 0;
	uint8_t nCHRBanks = 0;
	uint8_t mirrormode = 0x01;
};

