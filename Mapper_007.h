#pragma once
#include "Mapper.h"
class Mapper_007 : public Mapper
{
public:
	Mapper_007(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_007() {}
	bool CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write = false) override;
	bool PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write = false) override;
	void reset() override {nPRGBank = 0;}
	uint8_t mirror() override {return mirrormode;}
	bool irqState() override { return 0;}
	void irqClear() override {}
	void scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control) override {}

private:
	uint8_t nPRGBank = 0;
	uint8_t mirrormode = 0x0C;
};

