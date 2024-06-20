#include "Mapper_002.h"

Mapper_002::Mapper_002(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks){}

Mapper_002::~Mapper_002(){}

void Mapper_002::LoadState(uint8_t * state) {
	memcpy(&reg, state, sizeof(mapper));
};

uint8_t * Mapper_002::SaveState() {
	uint8_t * result = new uint8_t[sizeof(mapper)];
	memcpy(result, &reg, sizeof(mapper));
	return result;
};


void Mapper_002::reset() {
	reg.nPRGBankSelectLo = 0;
	reg.nPRGBankSelectHi = nPRGBanks - 1;
}

bool Mapper_002::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (write) reg.nPRGBankSelectLo = data & 0x0F;
		else {
			mapped_addr = ((addr<0xC000)?reg.nPRGBankSelectLo:reg.nPRGBankSelectHi) * 0x4000 + (addr & 0x3FFF);
			return true;
		}
	}
	return false;
}

bool Mapper_002::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = addr;
	return addr<0x2000;
}