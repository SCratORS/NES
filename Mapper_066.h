#pragma once
#include "Mapper.h"
class Mapper_066 :	public Mapper
{
public:
	Mapper_066(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_066();
	bool CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write = false) override;
	bool PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write = false) override;
	void reset() override;
	uint8_t mirror() override {return 0x01;}
	bool irqState() override { return 0;}
	void irqClear() override {}
	void scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control) override {}

private:
	uint8_t nCHRBankSelect = 0x00;
	uint8_t nPRGBankSelect = 0x00;
};

