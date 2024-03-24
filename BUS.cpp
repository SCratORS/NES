#include "BUS.h"

BUS::BUS() {
	CPU.ConnectBus(this);
	PPU.ConnectBus(this);
	APU.ConnectBus(this);
	for(uint16_t a=0; a<0x800; a++) RAM[a] = (a&4) ? 0xFF : 0x00;
}

BUS::~BUS() {
}

void BUS::Reset()
{	
	CART->reset();
	CPU.reset();
	PPU.reset();
	nSystemClockCounter = 0;
}


uint8_t BUS::MemAccess(uint16_t addr, uint8_t data, bool write) {
	
	if (addr < 0x2000)	{ uint8_t& M = RAM[addr & 0x07FF]; if (write) M = data; else return M; }
	else if (addr < 0x4000)	return PPU.MemAccess(addr&0x07, data, write);
	else if (addr < 0x4018)	switch(addr & 0x1F) {
								case 0x14: 
								case 0x16: 
								case 0x17: 	if (write) APU.MemAccess(addr&0x1F, data, write);
											return CPU.MemAccess(addr&0x1F, data, write);
								default: 	return APU.MemAccess(addr&0x1F, data, write);
							}
	else CART->MemAccess(addr, data, write);
	return data;
}

void BUS::Clock() {
	for(unsigned n=0; n<3; ++n) {
		PPU.clock();
		CPU.dma_clock(nSystemClockCounter++);
	}
	APU.clock();
	CPU.clock();
	if (PPU.nmi) {
		PPU.nmi = false;
		CPU.nmi();
	}
	if (CART->IRQState()) {
		CART->IRQClear();
		CPU.irq();		
	}
}

void BUS::ConnectCartridge(CARTRIDGE * cartridge) {
	this->CART = cartridge;
	PPU.ConnectCartridge(cartridge);
}		