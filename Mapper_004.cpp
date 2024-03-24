#include "Mapper_004.h"

Mapper_004::Mapper_004(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {
	vRAMStatic.resize(32 * 1024);
}

bool Mapper_004::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (addr < 0x6000) return false;
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		mapped_addr = 0xFFFFFFFF;
		uint8_t& M = vRAMStatic[addr & 0x1FFF];
		if (write) M = data; else data = M;
		return true;
	}
	if (write){
		switch ((addr & 1) + ((addr >> 12) & 6)) {
			case 0: nReg = data; break;
			case 1: pRegister[nReg&7] = data; break;
			case 2:	mirrormode = (data&1)?0x0C:0x0A; break;
			case 4: nIRQReload = data; break;
			case 5:	nIRQCounter = 0; bIRQReloadPending = true; break;
			case 6: bIRQEnable = bIRQActive = false; break;
			case 7: bIRQEnable = true; break;
		}
		for (uint8_t n=0; n<4; n++) {
			pCHRBank[n+((nReg&0x80)?0:4)] = pRegister[n+2] * 0x0400;
			pCHRBank[n+((nReg&0x80)?4:0)] = (pRegister[n>>1]+((n%2)?1:0)) * 0x0400;
		}
		pPRGBank[(nReg&0x40)?0:2] = (nPRGBanks * 2 - 2) * 0x2000;
		pPRGBank[(nReg&0x40)?2:0] = (pRegister[6] & lPRGBanks) * 0x2000;
		pPRGBank[1] = (pRegister[7] & lPRGBanks) * 0x2000;
	} else mapped_addr = pPRGBank[(addr>>13) & 3] + (addr & 0x1FFF);
	return !write;
}

bool Mapper_004::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = pCHRBank[addr>>10] + (addr & 0x03FF);
	return addr<0x2000;
}

void Mapper_004::reset() {
	nReg = 0x00;
	mirrormode = 0x0C;
	bIRQActive = bIRQEnable = false;
	nIRQCounter = nIRQReload = 0;
	for (int i = 0; i < 4; i++)	pPRGBank[i] = 0;
	for (int i = 0; i < 8; i++) { pCHRBank[i] = 0; pRegister[i] = 0xFF; }
	lPRGBanks = nPRGBanks * 2 - 1;
	pPRGBank[0] = 0 * 0x2000;
	pPRGBank[1] = 1 * 0x2000;
	pPRGBank[2] = (nPRGBanks * 2 - 2) * 0x2000;
	pPRGBank[3] = lPRGBanks * 0x2000;
}

void Mapper_004::scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control) {
	if (scanline < 240) {
		if (cycle == 260 && mask&0x18) {
			if (nIRQCounter == 0 || bIRQReloadPending) {
				nIRQCounter = nIRQReload;
				bIRQReloadPending = false;
			} else nIRQCounter--;
			if (nIRQCounter == 0 && bIRQEnable) bIRQActive = true;
		}
	}
}