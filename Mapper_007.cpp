 #include "Mapper_007.h"

Mapper_007::Mapper_007(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks){}

bool Mapper_007::CPUMapAddress(uint16_t addr, uint32_t &mapped_addr, uint8_t &data, bool write) {
	if (addr < 0x8000) return false;
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (write) {
			nPRGBank = data&7;
			mirrormode = ((data>>4)&1)?0xF:0;
		} else mapped_addr = nPRGBank * 0x8000 + (addr & 0x7FFF);
	}
	return !write;
}

bool Mapper_007::PPUMapAddress(uint16_t addr, uint32_t &mapped_addr, bool write) {
	mapped_addr = addr;
	return addr<0x2000;
}