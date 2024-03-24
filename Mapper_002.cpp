#include "Mapper_002.h"

Mapper_002::Mapper_002(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks){}

Mapper_002::~Mapper_002(){}

void Mapper_002::reset() {
	nPRGBankSelectLo = 0;
	nPRGBankSelectHi = nPRGBanks - 1;
}

bool Mapper_002::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (write) nPRGBankSelectLo = data & 0x0F;
		else {
			mapped_addr = ((addr<0xC000)?nPRGBankSelectLo:nPRGBankSelectHi) * 0x4000 + (addr & 0x3FFF);
			return true;
		}
	}
	return false;
}

bool Mapper_002::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = addr;
	return addr<0x2000;
}