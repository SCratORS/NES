 #include "Mapper_212.h"

Mapper_212::Mapper_212(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {}

Mapper_212::~Mapper_212() {}

void Mapper_212::LoadState(uint8_t * state) {
	memcpy(&reg, state, sizeof(mapper));
};

uint8_t * Mapper_212::SaveState() {
	uint8_t * result = new uint8_t[sizeof(mapper)];
	memcpy(result, &reg, sizeof(mapper));
	return result;
};

bool Mapper_212::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (write) {
		if (addr >= 0x8000 && addr <= 0xFFFF) {
			reg.bankingStyle = addr&0x4000;
			reg.nCHRBankSelect = reg.nPRGBankSelect = addr & 0x07;
			if (reg.bankingStyle) reg.nPRGBankSelect >>= 1;
			reg.mirrormode = addr&0x08?0x0C:0x0A;
		}
	} else {
		if ((addr & 0xE010) == 0x6000) {
			data |= ((addr & 0x10)? 0x00: 0x80);
			return false;
		}
		if (addr >= 0x8000) {
			if (reg.bankingStyle) mapped_addr = 0x8000 * reg.nPRGBankSelect + (addr & 0x7FFF);
			else mapped_addr = 0x4000 * reg.nPRGBankSelect + (addr & 0x3FFF);
			return true;
		}

	}
	return false;
}

bool Mapper_212::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = 0x2000 * reg.nCHRBankSelect + (addr & 0x1FFF);
	return addr < 0x2000;
}

void Mapper_212::reset() {
	reg.nPRGBankSelect = 0;
	reg.nCHRBankSelect = 0;
	reg.bankingStyle = false;
}
