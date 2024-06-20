#include "Mapper_023.h"
Mapper_023::Mapper_023(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {}

Mapper_023::~Mapper_023() {}

void Mapper_023::LoadState(uint8_t * state) {
	memcpy(&reg, state, sizeof(mapper));
};

uint8_t * Mapper_023::SaveState() {
	uint8_t * result = new uint8_t[sizeof(mapper)];
	memcpy(result, &reg, sizeof(mapper));
	return result;
};


bool Mapper_023::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {	
	if (addr >= 0x6000 && addr <= 0x7FFF) {
		mapped_addr = 0xFFFFFFFF;
		uint8_t& M = reg.vRAMStatic[addr & 0x1FFF];
		if (write) M = data; else data = M;
		return true;
	}
	if (addr>= 0x8000 && addr <= 0xFFFF) {
		if (write) {
			switch (addr&0xF000) {
				case 0x8000:	reg.nPRGBank[0] = data&0xF; break;
				case 0x9000:	switch(addr&0xF){
									case 0x02:	reg.wRAM = data&1;
												reg.mSWAP = data&2;
									default: 	data %= 0xFE;
												mirrormode = data&1?0x0C:0x0A;
												if (data>>1) mirrormode = (data&1)?0x0F:0x00;
								}
								break;
				case 0xA000:	reg.nPRGBank[1] = data&0xF; break;
				case 0xF000:	switch(addr&0xF){
									case 0: reg.nIRQReload &= 0xF0; reg.nIRQReload |= data & 0xF; break;
									case 1: reg.nIRQReload &= 0x0F; reg.nIRQReload |= data << 4; break;
									case 2: reg.nIRQCounter = reg.nIRQReload;
											reg.bIRQEnableAfter = data&1;
											reg.bIRQEnable = data&2;
											reg.bIRQMode = data&4;
											break;
									case 3: reg.bIRQEnable = reg.bIRQEnableAfter;
											break;
								}
								break;
				default:		uint8_t n =  ((addr-0xB000)>>11)+(((addr>>1)&1)|((addr>>3)&1));
								switch (addr&0x0F) {
									case 0:	case 2:	case 8: reg.nCHRBank[n] = data; break;
									default: reg.nCHRBank[n] = reg.nCHRBank[n] | data << 4;
								}
			}
		} else {
			if (reg.mSWAP) {
				if (addr >= 0xC000 && addr <= 0xDFFF) mapped_addr = reg.nPRGBank[0] * 0x2000 + (addr & 0x1FFF);
				if (addr >= 0x8000 && addr <= 0x9FFF) mapped_addr = (nPRGBanks * 2 - 2) * 0x2000 + (addr & 0x1FFF);
			} else {
				if (addr >= 0x8000 && addr <= 0x9FFF) mapped_addr = reg.nPRGBank[0] * 0x2000 + (addr & 0x1FFF);
				if (addr >= 0xC000 && addr <= 0xFFFF) mapped_addr = (nPRGBanks - 1) * 0x4000 + (addr & 0x3FFF);
			}
			if (addr >= 0xA000 && addr <= 0xBFFF) mapped_addr = reg.nPRGBank[1] * 0x2000 + (addr & 0x1FFF);
			return true;
		}
	}
	return false;
}

bool Mapper_023::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = reg.nCHRBank[addr>>10] * 0x0400 + (addr & 0x03FF);
	return addr<0x2000;
}

void Mapper_023::reset() {
	for (uint8_t i = 0; i < 2; i++)	reg.nPRGBank[i] = i;
	for (uint8_t i = 0; i < 8; i++)	reg.nCHRBank[i] = 0;
	for (uint16_t i = 0; i < 0x2000; i++) reg.vRAMStatic[i] = 0x00;
	reg.wRAM = false;
	reg.mSWAP = false;

	reg.bIRQActive = false;
	reg.bIRQMode = false;
	reg.bIRQEnableAfter = false;
	reg.bIRQEnable = false;

	reg.nIRQCounter = 0x0000;
	reg.nIRQReload = 0x0000;
}

uint8_t Mapper_023::mirror() {
	return mirrormode;
}
/*
bool Mapper_023::irqState() {
	return reg.bIRQActive;
}

void Mapper_023::irqClear() {
	reg.bIRQActive = false;
}

void Mapper_023::scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control) {
	if (scanline < 240) {
		if (cycle == 260 && mask&0x18) {
			if (!reg.bIRQMode) {
				if (reg.nIRQCounter == 0) reg.nIRQCounter = reg.nIRQReload;
				else reg.nIRQCounter--;
				if (reg.nIRQCounter == 0 && reg.bIRQEnable)	reg.bIRQActive = true;
			}
		}
	}
}
*/