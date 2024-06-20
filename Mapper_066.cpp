#include "Mapper_066.h"

Mapper_066::Mapper_066(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks){}

Mapper_066::~Mapper_066(){}

void Mapper_066::LoadState(uint8_t * state) {
	memcpy(&reg, state, sizeof(mapper));
};

uint8_t * Mapper_066::SaveState() {
	uint8_t * result = new uint8_t[sizeof(mapper)];
	memcpy(result, &reg, sizeof(mapper));
	return result;
};

bool Mapper_066::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (write) {
			reg.nCHRBankSelect = data & 0x03;
			reg.nPRGBankSelect = (data & 0x30) >> 4;
		} else {
			mapped_addr = reg.nPRGBankSelect * 0x8000 + (addr & 0x7FFF);
			return true;
		}
	}
	return false;
}

bool Mapper_066::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = reg.nCHRBankSelect * 0x2000 + addr;
	return addr<0x2000;
}

void Mapper_066::reset() {
	reg.nCHRBankSelect = 0;
	reg.nPRGBankSelect = 0;
}
