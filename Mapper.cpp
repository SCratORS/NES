#include "Mapper.h"

Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks) {
	nPRGBanks = prgBanks;
	nCHRBanks = chrBanks;
}

Mapper::~Mapper() {}

void Mapper::reset() {}

uint8_t Mapper::mirror() {return mirrormode;}
bool Mapper::irqState() {return false;}
void Mapper::irqClear() {}
void Mapper::scanline(int16_t cycle, int16_t scanline, uint8_t mask, uint8_t control){}
