#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
}


Mapper_000::~Mapper_000()
{
}

void Mapper_000::reset()
{

}

bool Mapper_000::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write)
{
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}
	return false;
}

bool Mapper_000::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {	
	mapped_addr = addr;
	return addr<0x2000;
}