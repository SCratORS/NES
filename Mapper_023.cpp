#include "Mapper_023.h"
Mapper_023::Mapper_023(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {}

Mapper_023::~Mapper_023() {}

bool Mapper_023::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {	
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		mapped_addr = 0xFFFFFFFF;
		uint8_t& M = vRAMStatic[addr & 0x1FFF];
		if (write) M = data; else data = M;
		return true;
	}
	if (addr>= 0x8000 && addr <= 0xFFFF) {
		if (write) {
			switch (addr&0xF000) {
				case 0x8000:	nPRGBank[0] = data&0xF; break;
				case 0x9000:	switch(addr&0xF){
									case 0x02:	wRAM = data&1;
												mSWAP = data&2;
									default: 	data %= 0xFE;
												mirrormode = data&1?0x0C:0x0A;
												if (data>>1) mirrormode = (data&1)?0x0F:0x00;
								}
								break;
				case 0xA000:	nPRGBank[1] = data&0xF; break;
				case 0xF000:	switch(addr&0xF){
									case 0: nIRQReload &= 0xF0; nIRQReload |= data & 0xF; break;
									case 1: nIRQReload &= 0x0F; nIRQReload |= data << 4; break;
									case 2: nIRQCounter = nIRQReload;
											bIRQEnableAfter = data&1;
											bIRQEnable = data&2;
											bIRQMode = data&4;
											break;
									case 3: bIRQEnable = bIRQEnableAfter;
											break;
								}
								break;
				default:		uint8_t n =  ((addr-0xB000)>>11)+(((addr>>1)&1)|((addr>>3)&1));
								switch (addr&0x0F) {
									case 0:	case 2:	case 8: nCHRBank[n] = data; break;
									default: nCHRBank[n] = nCHRBank[n] | data << 4;
								}
			}
		} else {
			if (mSWAP) {
				if (addr >= 0xC000 && addr <= 0xDFFF) mapped_addr = nPRGBank[0] * 0x2000 + (addr & 0x1FFF);
				if (addr >= 0x8000 && addr <= 0x9FFF) mapped_addr = (nPRGBanks * 2 - 2) * 0x2000 + (addr & 0x1FFF);
			} else {
				if (addr >= 0x8000 && addr <= 0x9FFF) mapped_addr = nPRGBank[0] * 0x2000 + (addr & 0x1FFF);
				if (addr >= 0xC000 && addr <= 0xFFFF) mapped_addr = (nPRGBanks - 1) * 0x4000 + (addr & 0x3FFF);
			}
			if (addr >= 0xA000 && addr <= 0xBFFF) mapped_addr = nPRGBank[1] * 0x2000 + (addr & 0x1FFF);
			return true;
		}
	}
	return false;
}

bool Mapper_023::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = nCHRBank[addr>>10] * 0x0400 + (addr & 0x03FF);
	return addr<0x2000;
}

void Mapper_023::reset() {
	for (uint8_t i = 0; i < 2; i++)	nPRGBank[i] = i;
	for (uint8_t i = 0; i < 8; i++)	nCHRBank[i] = 0;
	for (uint16_t i = 0; i < 0x2000; i++) vRAMStatic[i] = 0x00;
	wRAM = false;
	mSWAP = false;

	bIRQActive = false;
	bIRQMode = false;
	bIRQEnableAfter = false;
	bIRQEnable = false;

	nIRQCounter = 0x0000;
	nIRQReload = 0x0000;
}

uint8_t Mapper_023::mirror() {
	return mirrormode;
}
/*
bool Mapper_023::irqState() {
	return bIRQActive;
}

void Mapper_023::irqClear() {
	bIRQActive = false;
}

void Mapper_023::scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control) {
	if (scanline < 240) {
		if (cycle == 260 && mask&0x18) {
			if (!bIRQMode) {
				if (nIRQCounter == 0) nIRQCounter = nIRQReload;
				else nIRQCounter--;
				if (nIRQCounter == 0 && bIRQEnable)	bIRQActive = true;
			}
		}
	}
}
*/