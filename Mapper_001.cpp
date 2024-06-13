#include "Mapper_001.h"


Mapper_001::Mapper_001(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {
	vRAMStatic.resize(0x8000);
}

Mapper_001::~Mapper_001(){}

bool Mapper_001::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write)
{
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		mapped_addr = 0xFFFFFFFF;
		uint8_t& M = vRAMStatic[addr & 0x1FFF];
		if (write) M = data; else data = M;
		return true;
	}
	if (addr >= 0x8000) {
		if (!write) {
			if (nControlRegister & 0b01000) {
				if (addr >= 0x8000 && addr <= 0xBFFF) mapped_addr = nPRGBankSelect16Lo * 0x4000 + (addr & 0x3FFF);
				if (addr >= 0xC000 && addr <= 0xFFFF) mapped_addr = nPRGBankSelect16Hi * 0x4000 + (addr & 0x3FFF);
			} else mapped_addr = nPRGBankSelect32 * 0x8000 + (addr & 0x7FFF);
			return true;
		} else {
			if (data & 0x80) {
				nLoadRegister = 0x00;
				nLoadRegisterCount = 0;
				nControlRegister = nControlRegister | 0x0C;
			} else	{
				nLoadRegister >>= 1;
				nLoadRegister |= (data & 0x01) << 4;
				nLoadRegisterCount++;
				if (nLoadRegisterCount == 5) {
					switch ((addr >> 13) & 0x03) {
						case 0x00:	nControlRegister = nLoadRegister & 0x1F;
									switch (nControlRegister & 0x03){
										case 0:	mirrormode = 0x00; break;
										case 1: mirrormode = 0xFF; break;
										case 2: mirrormode = 0x0A; break;
										case 3:	mirrormode = 0x0C; break;
									}
						break;
						case 0x01:	if (nControlRegister & 0b10000) nCHRBankSelect4Lo = nLoadRegister & 0x1F;
									else nCHRBankSelect8 = nLoadRegister & 0x1E;
						break;
						case 0x02:	if (nControlRegister & 0b10000) nCHRBankSelect4Hi = nLoadRegister & 0x1F;
						break;
						case 0x03:	switch ((nControlRegister >> 2) & 0x03) {
										case 0x00:
										case 0x01:	nPRGBankSelect32 = (nLoadRegister & 0x0E) >> 1;
										break;
										case 0x02:	nPRGBankSelect16Lo = 0;
													nPRGBankSelect16Hi = nLoadRegister & 0x0F;
										break;
										case 0x03:	nPRGBankSelect16Lo = nLoadRegister & 0x0F;
													nPRGBankSelect16Hi = nPRGBanks - 1;
										break;
									}
						break;
					}
					nLoadRegister = 0x00;
					nLoadRegisterCount = 0;
				}
			}
		}
	}	
	return false;
}

bool Mapper_001::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	if (!nCHRBanks)	mapped_addr = addr;
	else if (nControlRegister & 0b10000) mapped_addr = ((addr<0x1000)?nCHRBankSelect4Lo:nCHRBankSelect4Hi) * 0x1000 + (addr & 0x0FFF);
	else mapped_addr = nCHRBankSelect8 * 0x1000 + (addr & 0x1FFF);			
	return addr<0x2000;
}

void Mapper_001::reset() {
	nControlRegister = 0x1C;
	nLoadRegister = 0x00;
	nLoadRegisterCount = 0x00;
	nCHRBankSelect4Lo = 0;
	nCHRBankSelect4Hi = 0;
	nCHRBankSelect8 = 0;
	nPRGBankSelect32 = 0;
	nPRGBankSelect16Lo = 0;
	nPRGBankSelect16Hi = nPRGBanks - 1;
}
