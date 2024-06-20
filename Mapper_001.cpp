#include "Mapper_001.h"

Mapper_001::Mapper_001(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {}

Mapper_001::~Mapper_001(){}

void Mapper_001::LoadState(uint8_t * state) {
	memcpy(&reg, state, sizeof(mapper));
};

uint8_t * Mapper_001::SaveState() {
	uint8_t * result = new uint8_t[sizeof(mapper)];
	memcpy(result, &reg, sizeof(mapper));
	return result;
};

bool Mapper_001::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write)
{
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		mapped_addr = 0xFFFFFFFF;
		uint8_t& M = reg.vRAMStatic[addr & 0x1FFF];
		if (write) M = data; else data = M;
		return true;
	}
	if (addr >= 0x8000) {
		if (!write) {
			if (reg.nControlRegister & 0b01000) {
				if (addr >= 0x8000 && addr <= 0xBFFF) mapped_addr = reg.nPRGBankSelect16Lo * 0x4000 + (addr & 0x3FFF);
				if (addr >= 0xC000 && addr <= 0xFFFF) mapped_addr = reg.nPRGBankSelect16Hi * 0x4000 + (addr & 0x3FFF);
			} else mapped_addr = reg.nPRGBankSelect32 * 0x8000 + (addr & 0x7FFF);
			return true;
		} else {
			if (data & 0x80) {
				reg.nLoadRegister = 0x00;
				reg.nLoadRegisterCount = 0;
				reg.nControlRegister = reg.nControlRegister | 0x0C;
			} else	{
				reg.nLoadRegister >>= 1;
				reg.nLoadRegister |= (data & 0x01) << 4;
				reg.nLoadRegisterCount++;
				if (reg.nLoadRegisterCount == 5) {
					switch ((addr >> 13) & 0x03) {
						case 0x00:	reg.nControlRegister = reg.nLoadRegister & 0x1F;
									switch (reg.nControlRegister & 0x03){
										case 0:	reg.mirrormode = 0x00; break;
										case 1: reg.mirrormode = 0xFF; break;
										case 2: reg.mirrormode = 0x0A; break;
										case 3:	reg.mirrormode = 0x0C; break;
									}
						break;
						case 0x01:	if (reg.nControlRegister & 0b10000) reg.nCHRBankSelect4Lo = reg.nLoadRegister & 0x1F;
									else reg.nCHRBankSelect8 = reg.nLoadRegister & 0x1E;
						break;
						case 0x02:	if (reg.nControlRegister & 0b10000) reg.nCHRBankSelect4Hi = reg.nLoadRegister & 0x1F;
						break;
						case 0x03:	switch ((reg.nControlRegister >> 2) & 0x03) {
										case 0x00:
										case 0x01:	reg.nPRGBankSelect32 = (reg.nLoadRegister & 0x0E) >> 1;
										break;
										case 0x02:	reg.nPRGBankSelect16Lo = 0;
													reg.nPRGBankSelect16Hi = reg.nLoadRegister & 0x0F;
										break;
										case 0x03:	reg.nPRGBankSelect16Lo = reg.nLoadRegister & 0x0F;
													reg.nPRGBankSelect16Hi = nPRGBanks - 1;
										break;
									}
						break;
					}
					reg.nLoadRegister = 0x00;
					reg.nLoadRegisterCount = 0;
				}
			}
		}
	}	
	return false;
}

bool Mapper_001::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	if (!nCHRBanks)	mapped_addr = addr;
	else if (reg.nControlRegister & 0b10000) mapped_addr = ((addr<0x1000)?reg.nCHRBankSelect4Lo:reg.nCHRBankSelect4Hi) * 0x1000 + (addr & 0x0FFF);
	else mapped_addr = reg.nCHRBankSelect8 * 0x1000 + (addr & 0x1FFF);			
	return addr<0x2000;
}

void Mapper_001::reset() {
	reg.nControlRegister = 0x1C;
	reg.nLoadRegister = 0x00;
	reg.nLoadRegisterCount = 0x00;
	reg.nCHRBankSelect4Lo = 0;
	reg.nCHRBankSelect4Hi = 0;
	reg.nCHRBankSelect8 = 0;
	reg.nPRGBankSelect32 = 0;
	reg.nPRGBankSelect16Lo = 0;
	reg.nPRGBankSelect16Hi = nPRGBanks - 1;
	for (uint16_t i = 0; i<0x8000; i++) reg.vRAMStatic[i] = 0x00; 
}
