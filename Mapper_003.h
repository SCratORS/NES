#pragma once
#include "Mapper.h"


class Mapper_003 : public Mapper
{
public:
	Mapper_003(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_003();

	bool CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write = false) override;
	bool PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write = false) override;
	void reset() override {reg.nCHRBankSelect = 0;}
	uint8_t mirror() override {return 0x01;}
	bool irqState() override { return 0;}
	void irqClear() override {}
	void LoadState(uint8_t * state) override;
	uint8_t * SaveState() override;
	uint32_t GetMapperSize() override { return sizeof(mapper);}
	void scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control) override {}
private:
	struct mapper {
		uint8_t nCHRBankSelect = 0x00;
	} reg;

};

