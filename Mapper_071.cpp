#include "Mapper_071.h"

Mapper_071::Mapper_071(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {}

Mapper_071::~Mapper_071() {}

void Mapper_071::LoadState(uint8_t * state) {
	memcpy(&reg, state, sizeof(mapper));
};

uint8_t * Mapper_071::SaveState() {
	uint8_t * result = new uint8_t[sizeof(mapper)];
	memcpy(result, &reg, sizeof(mapper));
	return result;
};

void Mapper_071::reset() {
	reg.nPRGBankSelectLo = 0;
	reg.nPRGBankSelectHi = nPRGBanks - 1;
}

bool Mapper_071::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (write) {
			if ((addr & 0xF000) == 0x9000) mirrormode = data&4?0x0C:0x0A;
			else reg.nPRGBankSelectLo = data & 0x0F;
		} else {
			if (addr >= 0x8000 && addr <= 0xBFFF) {
				mapped_addr = reg.nPRGBankSelectLo * 0x4000 + (addr & 0x3FFF);
				return true;
			}
			if (addr >= 0xC000 && addr <= 0xFFFF) {
				mapped_addr = reg.nPRGBankSelectHi * 0x4000 + (addr & 0x3FFF);
				return true;
			}
		}
	}
	return false;
}

bool Mapper_071::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = addr;
	return addr<0x2000;
}
