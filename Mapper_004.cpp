#include "Mapper_004.h"
#include <cstdio>
Mapper_004::Mapper_004(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {
}

void Mapper_004::LoadState(uint8_t * state) {
	memcpy(&reg, state, sizeof(mapper));
};

uint8_t * Mapper_004::SaveState() {
	uint8_t * result = new uint8_t[sizeof(mapper)];
	memcpy(result, &reg, sizeof(mapper));
	return result;
};

bool Mapper_004::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (addr < 0x6000) return false;
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		mapped_addr = 0xFFFFFFFF;
		uint8_t& M = reg.vRAMStatic[addr & 0x1FFF];
		if (write) M = data; else data = M;
		return true;
	}
	if (write){
		switch ((addr & 1) + ((addr >> 12) & 6)) {
			case 0: reg.nReg = data; break;
			case 1: reg.pRegister[reg.nReg&7] = data; break;
			case 2:	reg.mirrormode = (data&1)?0x0C:0x0A; break;
			case 4: reg.nIRQReload = data; break;
			case 5:	reg.nIRQCounter = 0; reg.bIRQReloadPending = true; break;
			case 6: reg.bIRQEnable = reg.bIRQActive = false; break;
			case 7: reg.bIRQEnable = true; break;
		}
		for (uint8_t n=0; n<4; n++) {
			reg.pCHRBank[n+((reg.nReg&0x80)?0:4)] = reg.pRegister[n+2] * 0x0400;
			reg.pCHRBank[n+((reg.nReg&0x80)?4:0)] = (reg.pRegister[n>>1]+((n%2)?1:0)) * 0x0400;
		}
		reg.pPRGBank[(reg.nReg&0x40)?0:2] = (nPRGBanks * 2 - 2) * 0x2000;
		reg.pPRGBank[(reg.nReg&0x40)?2:0] = (reg.pRegister[6] & reg.lPRGBanks) * 0x2000;
		reg.pPRGBank[1] = (reg.pRegister[7] & reg.lPRGBanks) * 0x2000;
	} else mapped_addr = reg.pPRGBank[(addr>>13) & 3] + (addr & 0x1FFF);
	return !write;
}

bool Mapper_004::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = reg.pCHRBank[addr>>10] + (addr & 0x03FF);
	return addr<0x2000;
}

void Mapper_004::reset() {
	reg.nReg = 0x00;
	reg.mirrormode = 0x0C;
	reg.bIRQActive = reg.bIRQEnable = false;
	reg.nIRQCounter = reg.nIRQReload = 0;
	for (int i = 0; i < 4; i++)	reg.pPRGBank[i] = 0;
	for (int i = 0; i < 8; i++) { reg.pCHRBank[i] = 0; reg.pRegister[i] = 0x00; }
	reg.lPRGBanks = nPRGBanks * 2 - 1;
	reg.pPRGBank[0] = 0 * 0x2000;
	reg.pPRGBank[1] = 1 * 0x2000;
	reg.pPRGBank[2] = (nPRGBanks * 2 - 2) * 0x2000;
	reg.pPRGBank[3] = reg.lPRGBanks * 0x2000;
	for (uint16_t i = 0; i<0x8000; i++) reg.vRAMStatic[i] = 0x00; 
}

void Mapper_004::scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control) {
	if (scanline < 240) {
		if (cycle == 260 && mask&0x18) {
			if (reg.nIRQCounter == 0 || reg.bIRQReloadPending) {
				reg.nIRQCounter = reg.nIRQReload;
				reg.bIRQReloadPending = false;
			} else reg.nIRQCounter--;
			if (reg.nIRQCounter == 0 && reg.bIRQEnable) reg.bIRQActive = true;
		}
	}
}