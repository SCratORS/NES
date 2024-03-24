#include "Mapper_066.h"

Mapper_066::Mapper_066(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks){}

Mapper_066::~Mapper_066(){}

bool Mapper_066::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (write) {
			nCHRBankSelect = data & 0x03;
			nPRGBankSelect = (data & 0x30) >> 4;
		} else {
			mapped_addr = nPRGBankSelect * 0x8000 + (addr & 0x7FFF);
			return true;
		}
	}
	return false;
}

bool Mapper_066::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = nCHRBankSelect * 0x2000 + addr;
	return addr<0x2000;
}

void Mapper_066::reset() {
	nCHRBankSelect = 0;
	nPRGBankSelect = 0;
}
