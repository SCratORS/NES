#pragma once
#include "Mapper.h"
#include <vector>

class Mapper_004 : public Mapper
{
public:
	Mapper_004(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_004() {}
	bool CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write = false) override;
	bool PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write = false) override;
	void reset() override;
	bool irqState() override {return bIRQActive;}
	void irqClear() override {bIRQActive = false;}
	void scanline(int16_t x, int16_t y, uint8_t mask, uint8_t control) override;
	uint8_t mirror() override {return mirrormode;}

private:
	uint8_t nReg = 0x00;
	uint8_t mirrormode = 0x0C;
	uint8_t lPRGBanks = 0;
	uint8_t pRegister[8];
	uint32_t pCHRBank[8];
	uint32_t pPRGBank[4];
	bool bIRQActive = false;
	bool bIRQEnable = false;
	bool bIRQUpdate = false;
	bool bIRQReloadPending = false;
	uint16_t nIRQCounter = 0x0000;
	uint16_t nIRQReload = 0x0000;
	std::vector<uint8_t> vRAMStatic;
};

