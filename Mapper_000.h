#pragma once
#include "Mapper.h"

class Mapper_000 : public Mapper
{
public:
	Mapper_000(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_000();

public:
	bool CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write = false) override;
	bool PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write = false) override;
	void reset() override;
	// No local equipment required
};

