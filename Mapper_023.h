#pragma once
#include "Mapper.h"


class Mapper_023 : public Mapper
{
public:
	Mapper_023(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_023();

	bool CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write = false) override;
	bool PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write = false) override;
	void reset() override;
	void LoadState(uint8_t * state) override;
	uint8_t * SaveState() override;
	uint32_t GetMapperSize() override { return sizeof(mapper);}
	/*
	bool irqState() override;
	void irqClear() override;
	void scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control) override;
	*/
	uint8_t mirror();

private:
	struct mapper {
		uint8_t nCHRBank[8];
		uint8_t nPRGBank[2];
		uint8_t mirrormode = 0x0A;
		uint8_t vRAMStatic[0x2000];
		bool wRAM = false;
		bool mSWAP = false;
		bool bIRQActive = false;
		bool bIRQMode = false;
		bool bIRQEnable = false;
		bool bIRQEnableAfter = false;
		uint16_t nIRQCounter = 0x0000;
		uint16_t nIRQReload = 0x0000;
	} reg;

};

