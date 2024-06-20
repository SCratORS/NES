#include "BUS.h"

BUS::BUS() {
	CPU.ConnectBus(this);
	PPU.ConnectBus(this);
	APU.ConnectBus(this);
}

BUS::~BUS() {
}

void BUS::Reset()
{	
	if (this->CART) CART->reset();
	CPU.reset();
	PPU.reset();
	APU.reset();
	for(uint16_t a=0; a<0x800; a++) RAM[a] = (a&4) ? 0xFF : 0x00;
	nSystemClockCounter = 0;
}

void BUS::SaveState(State * state) {
	memcpy(&state->RAM, &RAM, 0x800);
	state->nSystemClockCounter = nSystemClockCounter;
	CPU.SaveState(&state->CPU);
	PPU.SaveState(&state->PPU);
	APU.SaveState(&state->APU);
	CART->SaveState(&state->CART);
}

void BUS::LoadState(State * state) {
	memcpy(&RAM, &state->RAM, 0x800);
	nSystemClockCounter = state->nSystemClockCounter;
	CPU.LoadState(&state->CPU);
	PPU.LoadState(&state->PPU);
	APU.LoadState(&state->APU);
	CART->LoadState(&state->CART);
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
	if (PPU.nmi) {
		PPU.nmi = false;
		CPU.nmi();
	}
	if (CART->IRQState()) {
		CART->IRQClear();
		CPU.irq();		
	}
	APU.clock();
	CPU.clock();
}

void BUS::RejectCartridge() {
	PPU.RejectCartridge();
	this->CART = nullptr;
}

void BUS::ConnectCartridge(CARTRIDGE * cartridge) {
	this->CART = cartridge;
	PPU.ConnectCartridge(cartridge);
}		