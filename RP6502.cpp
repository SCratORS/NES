#include "BUS.h"
#include "RP6502.h"

#define Implied (lookup[opcode].addrmode == &RP6502::IMP)

// Constructor
RP6502::RP6502()
{
	lookup = new INSTRUCTION [0x100] 
	{	/*      0                   1                   2                   3                   4                   5                   6                   7                   8                   9                   A                   B                   C                     D                 E                     F*/
/*0*/	{&RP6502::BRK,&RP6502::IMP,7},{&RP6502::ORA,&RP6502::IZX,6},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::SLO,&RP6502::IZX,8},{&RP6502::NOP,&RP6502::ZP0,3},{&RP6502::ORA,&RP6502::ZP0,3},{&RP6502::ASL,&RP6502::ZP0,5},{&RP6502::SLO,&RP6502::ZP0,5},{&RP6502::PHP,&RP6502::IMP,3},{&RP6502::ORA,&RP6502::IMM,2},{&RP6502::ASL,&RP6502::IMP,2},{&RP6502::ANC,&RP6502::IMM,2},{&RP6502::NOP,&RP6502::ABS,4},{&RP6502::ORA,&RP6502::ABS,4},{&RP6502::ASL,&RP6502::ABS,6},{&RP6502::SLO, &RP6502::ABS,6},
/*1*/	{&RP6502::BPL,&RP6502::REL,2},{&RP6502::ORA,&RP6502::IZY,5},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::SLO,&RP6502::IZY,8},{&RP6502::NOP,&RP6502::ZPX,4},{&RP6502::ORA,&RP6502::ZPX,4},{&RP6502::ASL,&RP6502::ZPX,6},{&RP6502::SLO,&RP6502::ZPX,6},{&RP6502::CLC,&RP6502::IMP,2},{&RP6502::ORA,&RP6502::ABY,4},{&RP6502::NOP,&RP6502::IMP,2},{&RP6502::SLO,&RP6502::ABY,7},{&RP6502::NOP,&RP6502::ABX,4},{&RP6502::ORA,&RP6502::ABX,4},{&RP6502::ASL,&RP6502::ABX,7},{&RP6502::SLO, &RP6502::ABX,7},
/*2*/	{&RP6502::JSR,&RP6502::ABS,6},{&RP6502::AND,&RP6502::IZX,6},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::RLA,&RP6502::IZX,8},{&RP6502::BIT,&RP6502::ZP0,3},{&RP6502::AND,&RP6502::ZP0,3},{&RP6502::ROL,&RP6502::ZP0,5},{&RP6502::RLA,&RP6502::ZP0,5},{&RP6502::PLP,&RP6502::IMP,4},{&RP6502::AND,&RP6502::IMM,2},{&RP6502::ROL,&RP6502::IMP,2},{&RP6502::ANC,&RP6502::IMM,2},{&RP6502::BIT,&RP6502::ABS,4},{&RP6502::AND,&RP6502::ABS,4},{&RP6502::ROL,&RP6502::ABS,6},{&RP6502::RLA, &RP6502::ABS,6},
/*3*/	{&RP6502::BMI,&RP6502::REL,2},{&RP6502::AND,&RP6502::IZY,5},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::RLA,&RP6502::IZY,8},{&RP6502::NOP,&RP6502::ZPX,4},{&RP6502::AND,&RP6502::ZPX,4},{&RP6502::ROL,&RP6502::ZPX,6},{&RP6502::RLA,&RP6502::ZPX,6},{&RP6502::SEC,&RP6502::IMP,2},{&RP6502::AND,&RP6502::ABY,4},{&RP6502::NOP,&RP6502::IMP,2},{&RP6502::RLA,&RP6502::ABY,7},{&RP6502::NOP,&RP6502::ABX,4},{&RP6502::AND,&RP6502::ABX,4},{&RP6502::ROL,&RP6502::ABX,7},{&RP6502::RLA, &RP6502::ABX,7},
/*4*/	{&RP6502::RTI,&RP6502::IMP,6},{&RP6502::EOR,&RP6502::IZX,6},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::SRE,&RP6502::IZX,8},{&RP6502::NOP,&RP6502::ZP0,3},{&RP6502::EOR,&RP6502::ZP0,3},{&RP6502::LSR,&RP6502::ZP0,5},{&RP6502::SRE,&RP6502::ZP0,5},{&RP6502::PHA,&RP6502::IMP,3},{&RP6502::EOR,&RP6502::IMM,2},{&RP6502::LSR,&RP6502::IMP,2},{&RP6502::ALR,&RP6502::IMM,2},{&RP6502::JMP,&RP6502::ABS,3},{&RP6502::EOR,&RP6502::ABS,4},{&RP6502::LSR,&RP6502::ABS,6},{&RP6502::SRE, &RP6502::ABS,6},
/*5*/	{&RP6502::BVC,&RP6502::REL,2},{&RP6502::EOR,&RP6502::IZY,5},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::SRE,&RP6502::IZY,8},{&RP6502::NOP,&RP6502::ZPX,4},{&RP6502::EOR,&RP6502::ZPX,4},{&RP6502::LSR,&RP6502::ZPX,6},{&RP6502::SRE,&RP6502::ZPX,6},{&RP6502::CLI,&RP6502::IMP,2},{&RP6502::EOR,&RP6502::ABY,4},{&RP6502::NOP,&RP6502::IMP,2},{&RP6502::SRE,&RP6502::ABY,7},{&RP6502::NOP,&RP6502::ABX,4},{&RP6502::EOR,&RP6502::ABX,4},{&RP6502::LSR,&RP6502::ABX,7},{&RP6502::SRE, &RP6502::ABX,7},
/*6*/	{&RP6502::RTS,&RP6502::IMP,6},{&RP6502::ADC,&RP6502::IZX,6},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::RRA,&RP6502::IZX,8},{&RP6502::NOP,&RP6502::ZP0,3},{&RP6502::ADC,&RP6502::ZP0,3},{&RP6502::ROR,&RP6502::ZP0,5},{&RP6502::RRA,&RP6502::ZP0,5},{&RP6502::PLA,&RP6502::IMP,4},{&RP6502::ADC,&RP6502::IMM,2},{&RP6502::ROR,&RP6502::IMP,2},{&RP6502::ARR,&RP6502::IMM,2},{&RP6502::JMP,&RP6502::IND,5},{&RP6502::ADC,&RP6502::ABS,4},{&RP6502::ROR,&RP6502::ABS,6},{&RP6502::RRA, &RP6502::ABS,6},
/*7*/	{&RP6502::BVS,&RP6502::REL,2},{&RP6502::ADC,&RP6502::IZY,5},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::RRA,&RP6502::IZY,8},{&RP6502::NOP,&RP6502::ZPX,4},{&RP6502::ADC,&RP6502::ZPX,4},{&RP6502::ROR,&RP6502::ZPX,6},{&RP6502::RRA,&RP6502::ZPX,6},{&RP6502::SEI,&RP6502::IMP,2},{&RP6502::ADC,&RP6502::ABY,4},{&RP6502::NOP,&RP6502::IMP,2},{&RP6502::RRA,&RP6502::ABY,7},{&RP6502::NOP,&RP6502::ABX,4},{&RP6502::ADC,&RP6502::ABX,4},{&RP6502::ROR,&RP6502::ABX,7},{&RP6502::RRA, &RP6502::ABX,7},
/*8*/	{&RP6502::NOP,&RP6502::IMM,2},{&RP6502::STA,&RP6502::IZX,6},{&RP6502::NOP,&RP6502::IMM,2},{&RP6502::SAX,&RP6502::IZX,6},{&RP6502::STY,&RP6502::ZP0,3},{&RP6502::STA,&RP6502::ZP0,3},{&RP6502::STX,&RP6502::ZP0,3},{&RP6502::SAX,&RP6502::ZP0,3},{&RP6502::DEY,&RP6502::IMP,2},{&RP6502::NOP,&RP6502::IMM,2},{&RP6502::TXA,&RP6502::IMP,2},{&RP6502::XAA,&RP6502::IMM,2},{&RP6502::STY,&RP6502::ABS,4},{&RP6502::STA,&RP6502::ABS,4},{&RP6502::STX,&RP6502::ABS,4},{&RP6502::SAX, &RP6502::ABS,4},
/*9*/	{&RP6502::BCC,&RP6502::REL,2},{&RP6502::STA,&RP6502::IZY,6},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::AHX,&RP6502::IZY,6},{&RP6502::STY,&RP6502::ZPX,4},{&RP6502::STA,&RP6502::ZPX,4},{&RP6502::STX,&RP6502::ZPY,4},{&RP6502::SAX,&RP6502::ZPY,4},{&RP6502::TYA,&RP6502::IMP,2},{&RP6502::STA,&RP6502::ABY,5},{&RP6502::TXS,&RP6502::IMP,2},{&RP6502::TAS,&RP6502::ABY,5},{&RP6502::NOP,&RP6502::ABX,5},{&RP6502::STA,&RP6502::ABX,5},{&RP6502::SHX,&RP6502::ABY,5},{&RP6502::AHX, &RP6502::ABY,5},
/*A*/	{&RP6502::LDY,&RP6502::IMM,2},{&RP6502::LDA,&RP6502::IZX,6},{&RP6502::LDX,&RP6502::IMM,2},{&RP6502::LAX,&RP6502::IZX,6},{&RP6502::LDY,&RP6502::ZP0,3},{&RP6502::LDA,&RP6502::ZP0,3},{&RP6502::LDX,&RP6502::ZP0,3},{&RP6502::LAX,&RP6502::ZP0,3},{&RP6502::TAY,&RP6502::IMP,2},{&RP6502::LDA,&RP6502::IMM,2},{&RP6502::TAX,&RP6502::IMP,2},{&RP6502::LAX,&RP6502::IMM,2},{&RP6502::LDY,&RP6502::ABS,4},{&RP6502::LDA,&RP6502::ABS,4},{&RP6502::LDX,&RP6502::ABS,4},{&RP6502::LAX, &RP6502::ABS,4},
/*B*/	{&RP6502::BCS,&RP6502::REL,2},{&RP6502::LDA,&RP6502::IZY,5},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::LAX,&RP6502::IZY,5},{&RP6502::LDY,&RP6502::ZPX,4},{&RP6502::LDA,&RP6502::ZPX,4},{&RP6502::LDX,&RP6502::ZPY,4},{&RP6502::LAX,&RP6502::ZPY,4},{&RP6502::CLV,&RP6502::IMP,2},{&RP6502::LDA,&RP6502::ABY,4},{&RP6502::TSX,&RP6502::IMP,2},{&RP6502::LAS,&RP6502::ABY,4},{&RP6502::LDY,&RP6502::ABX,4},{&RP6502::LDA,&RP6502::ABX,4},{&RP6502::LDX,&RP6502::ABY,4},{&RP6502::LAX, &RP6502::ABY,4},
/*C*/	{&RP6502::CPY,&RP6502::IMM,2},{&RP6502::CMP,&RP6502::IZX,6},{&RP6502::NOP,&RP6502::IMM,2},{&RP6502::DCP,&RP6502::IZX,8},{&RP6502::CPY,&RP6502::ZP0,3},{&RP6502::CMP,&RP6502::ZP0,3},{&RP6502::DEC,&RP6502::ZP0,5},{&RP6502::DCP,&RP6502::ZP0,5},{&RP6502::INY,&RP6502::IMP,2},{&RP6502::CMP,&RP6502::IMM,2},{&RP6502::DEX,&RP6502::IMP,2},{&RP6502::AXS,&RP6502::IMM,2},{&RP6502::CPY,&RP6502::ABS,4},{&RP6502::CMP,&RP6502::ABS,4},{&RP6502::DEC,&RP6502::ABS,6},{&RP6502::DCP, &RP6502::ABS,6},
/*D*/	{&RP6502::BNE,&RP6502::REL,2},{&RP6502::CMP,&RP6502::IZY,5},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::DCP,&RP6502::IZY,8},{&RP6502::NOP,&RP6502::ZPX,4},{&RP6502::CMP,&RP6502::ZPX,4},{&RP6502::DEC,&RP6502::ZPX,6},{&RP6502::DCP,&RP6502::ZPX,6},{&RP6502::CLD,&RP6502::IMP,2},{&RP6502::CMP,&RP6502::ABY,4},{&RP6502::NOP,&RP6502::IMP,2},{&RP6502::DCP,&RP6502::ABY,7},{&RP6502::NOP,&RP6502::ABX,4},{&RP6502::CMP,&RP6502::ABX,4},{&RP6502::DEC,&RP6502::ABX,7},{&RP6502::DCP, &RP6502::ABX,7},
/*E*/	{&RP6502::CPX,&RP6502::IMM,2},{&RP6502::SBC,&RP6502::IZX,6},{&RP6502::NOP,&RP6502::IMM,2},{&RP6502::ISC,&RP6502::IZX,8},{&RP6502::CPX,&RP6502::ZP0,3},{&RP6502::SBC,&RP6502::ZP0,3},{&RP6502::INC,&RP6502::ZP0,5},{&RP6502::ISC,&RP6502::ZP0,5},{&RP6502::INX,&RP6502::IMP,2},{&RP6502::SBC,&RP6502::IMM,2},{&RP6502::NOP,&RP6502::IMP,2},{&RP6502::SBC,&RP6502::IMM,2},{&RP6502::CPX,&RP6502::ABS,4},{&RP6502::SBC,&RP6502::ABS,4},{&RP6502::INC,&RP6502::ABS,6},{&RP6502::ISC, &RP6502::ABS,6},
/*F*/	{&RP6502::BEQ,&RP6502::REL,2},{&RP6502::SBC,&RP6502::IZY,5},{&RP6502::KIL,&RP6502::IMP,2},{&RP6502::ISC,&RP6502::IZY,8},{&RP6502::NOP,&RP6502::ZPX,4},{&RP6502::SBC,&RP6502::ZPX,4},{&RP6502::INC,&RP6502::ZPX,6},{&RP6502::ISC,&RP6502::ZPX,6},{&RP6502::SED,&RP6502::IMP,2},{&RP6502::SBC,&RP6502::ABY,4},{&RP6502::NOP,&RP6502::IMP,2},{&RP6502::ISC,&RP6502::ABY,7},{&RP6502::NOP,&RP6502::ABX,4},{&RP6502::SBC,&RP6502::ABX,4},{&RP6502::INC,&RP6502::ABX,7},{&RP6502::ISC, &RP6502::ABX,7},
	};
}

RP6502::~RP6502(){}

void RP6502::SaveState(CPUState * state) {
	memcpy(state, &reg, sizeof(CPUState));
}

void RP6502::LoadState(CPUState * state) {
	memcpy(&reg, state, sizeof(CPUState));
}

uint8_t	 RP6502::MemAccess(uint16_t addr, uint8_t data, bool write) {
	uint8_t result = data;
	switch (addr) {
		case 0x14:	if (!write)	break;
					dma_page = data;
					dma_addr = 0x00;
					dma_transfer = true;
		break;
		case 0x16:	if (write) controller_state[addr & 0x0001] = controller[addr & 0x0001];
					else {
						result = (controller_state[addr & 0x0001] & 0x80) > 0;
						controller_state[addr & 0x0001] <<= 1;
					}
		break;
		case 0x17:	if (write) controller_state[addr & 0x0001] = controller[addr & 0x0001];
					else {
						result = (controller_state[addr & 0x0001] & 0x80) > 0;
						controller_state[addr & 0x0001] <<= 1;
					}
		break;
	}
	return result;
}

void RP6502::branch() {	addr_abs = reg.pc + addr_rel;	cycles += crosspage(addr_abs, reg.pc)?2:1;	reg.pc = addr_abs; }
void RP6502::setZN(uint8_t v) { SetFlag(Z, !v ); SetFlag(N, v&N); }
void RP6502::restore()	{ if (Implied) reg.a = reg.m;	else bus->MemAccess(addr_abs, reg.m, true);}
uint8_t RP6502::fetch()	{	if (!Implied) reg.m = bus->MemAccess(addr_abs);	return reg.m; }
uint8_t  RP6502::pull(uint8_t  &v)	{	v = bus->MemAccess(0x0100 + ++reg.stkp); return v; }
uint16_t RP6502::pull(uint16_t &v)	{ v = bus->MemAccess(0x0100 + ++reg.stkp) | bus->MemAccess(0x0100 + ++reg.stkp) << 8;	return v; }
void RP6502::push(uint8_t  v) {	bus->MemAccess(0x0100 + reg.stkp--, v, true); }
void RP6502::push(uint16_t v) {	bus->MemAccess(0x0100 + reg.stkp--, (v >> 8) & 0x00FF, true);	bus->MemAccess(0x0100 + reg.stkp--, v & 0x00FF, true); }
uint8_t RP6502::ab_(uint8_t v){ ZP0(); uint16_t addr_tmp = bus->MemAccess(reg.pc++)<<8;	addr_abs |= addr_tmp;	addr_abs += v;	return crosspage(addr_abs, addr_tmp)?1:0; }
uint8_t RP6502::zp_(uint8_t v){	ZP0(); addr_abs += v; addr_abs &= 0x00FF; return 0; }

void RP6502::reset() {
	reg.a = reg.x = reg.y =reg. m = 0x00;
	reg.status = 0x00 | U;
	reg.stkp = 0xFD;
	reg.pc = bus->MemAccess(0xFFFC) | (bus->MemAccess(0xFFFD) << 8);
	addr_rel = addr_abs = 0x0000;
	cycles = 8;
	dma_page = 0x00; dma_addr = 0x00; dma_data = 0x00;
	dma_dummy = true;
	dma_transfer = false;
}

void RP6502::irq() {
	if (GetFlag(I)) return;
	reg.status &= 0xFB; /* B = 0; U = 1; I = 1; */
	push(reg.pc);
	push(reg.status);
	addr_abs = 0xFFFE;
	reg.pc = (bus->MemAccess(addr_abs + 1) << 8) | bus->MemAccess(addr_abs + 0);
	cycles = 12;
}


void RP6502::nmi() {
	reg.status &= 0xFB; /* B = 0; U = 1; I = 1; */
	push(reg.pc);
	push(reg.status);
	addr_abs = 0xFFFA;
	reg.pc = (bus->MemAccess(addr_abs + 1) << 8) | bus->MemAccess(addr_abs + 0);
	cycles = 12; // 7 cycles + 5 cycles VBlankState
}

void RP6502::dma_clock(uint32_t counter) {
	if (counter % 3 == 0) {
		if (dma_transfer) {
			if (dma_dummy) {
				if (counter % 2 == 1) dma_dummy = false;
			} else {
				if (counter % 2 == 0) dma_data = bus->MemAccess(dma_page << 8 | dma_addr);
				else {
					bus->PPU.pOAM[dma_addr++] = dma_data;
					if (dma_addr == 0x00) {
						dma_transfer = false;
						dma_dummy = true;
					}
				}
			}
		}
	}
}


void RP6502::clock() {
	if (dma_transfer) return;
	if (!cycles) {
		opcode = bus->MemAccess(reg.pc++);
		reg.status |= 0x20;
		cycles = lookup[opcode].cycles;
		cycles += ((this->*lookup[opcode].addrmode)() & (this->*lookup[opcode].operate)());
		reg.status |= 0x20;
	}
	clock_count++;
	cycles--;
}

uint8_t RP6502::GetFlag(FLAGS6502 f) {
	return ((reg.status & f) > 0) ? 1 : 0;
}

void RP6502::SetFlag(FLAGS6502 f, bool v) {
	if (f==C) CF = GetFlag(C);
	if (v) 	reg.status |= f;
	else 	reg.status &= ~f;
}

uint8_t RP6502::IND() {
	uint16_t addr_tmp  =  bus->MemAccess(reg.pc++) | bus->MemAccess(reg.pc++) << 8;
	if ((addr_tmp & 0xFF) == 0xFF) 	addr_abs  = bus->MemAccess(addr_tmp & 0xFF00) << 8;
	else														addr_abs  = bus->MemAccess(addr_tmp + 0x0001) << 8;
																addr_abs |= bus->MemAccess(addr_tmp + 0x0000);
	return 0;
}
uint8_t RP6502::IZX() {
	uint16_t addr_tmp = bus->MemAccess(reg.pc++);
	addr_abs  = bus->MemAccess((uint16_t)(addr_tmp + (uint16_t)reg.x + 0) & 0x00FF);
	addr_abs |= bus->MemAccess((uint16_t)(addr_tmp + (uint16_t)reg.x + 1) & 0x00FF) << 8;
	return 0;
}
uint8_t RP6502::IZY() {
	uint16_t addr_tmp = bus->MemAccess(reg.pc++);
	addr_abs = bus->MemAccess((addr_tmp + 0) & 0x00FF);
	addr_tmp = bus->MemAccess((addr_tmp + 1) & 0x00FF) << 8;
	addr_abs |= addr_tmp;
	addr_abs += reg.y;
	return crosspage(addr_abs, addr_tmp)?1:0;
}
uint8_t RP6502::ABX() { return ab_(reg.x); }
uint8_t RP6502::ABY() { return ab_(reg.y); }
uint8_t RP6502::ZPX() { return zp_(reg.x); }
uint8_t RP6502::ZPY() { return zp_(reg.y); }
uint8_t RP6502::IMP() { reg.m = reg.a; return 0; }
uint8_t RP6502::IMM() { addr_abs = reg.pc++; return 0; }
uint8_t RP6502::ZP0() { addr_abs = bus->MemAccess(reg.pc++); return 0; }
uint8_t RP6502::ABS() { ZP0(); addr_abs |= bus->MemAccess(reg.pc++)<<8; return 0; }
uint8_t RP6502::REL() { addr_rel = bus->MemAccess(reg.pc++); if (addr_rel&N) addr_rel |= 0xFF00; return 0; }
/* OPCODES */
uint8_t RP6502::ADC() { fetch(); temp = reg.a + reg.m + GetFlag(C); SetFlag(C, temp & 0xFF00); SetFlag(V, (~(reg.m^reg.a)&(temp^reg.a)) & N); setZN(reg.a = temp); return 1; }
uint8_t RP6502::SBC() { fetch(); uint16_t v = reg.m ^ 0x00FF; temp = reg.a + v + GetFlag(C); SetFlag(C, temp & 0xFF00); SetFlag(V, (temp^reg.a)&(temp^v) & N); setZN(reg.a = temp); return 1; }
uint8_t RP6502::BRK() { SetFlag(I, 1); push(++reg.pc); push((uint8_t) (reg.status | B | U)); reg.pc = bus->MemAccess(0xFFFE) | (bus->MemAccess(0xFFFF) << 8); return 0; }
uint8_t RP6502::NOP() { return opcode&0x1C?1:0; }
uint8_t RP6502::BCC() { if (!GetFlag(C)) branch(); return 0; }
uint8_t RP6502::BCS() { if ( GetFlag(C)) branch(); return 0; }
uint8_t RP6502::BNE() { if (!GetFlag(Z)) branch(); return 0; }
uint8_t RP6502::BEQ() { if ( GetFlag(Z)) branch(); return 0; }
uint8_t RP6502::BPL() { if (!GetFlag(N)) branch(); return 0; }
uint8_t RP6502::BMI() { if ( GetFlag(N)) branch(); return 0; }
uint8_t RP6502::BVC() { if (!GetFlag(V)) branch(); return 0; }
uint8_t RP6502::BVS() { if ( GetFlag(V)) branch(); return 0; }
uint8_t RP6502::PHP() { push((uint8_t) (reg.status | B | U)); reg.status &= 0xCF; return 0; }
uint8_t RP6502::RTI() { pull(reg.status); pull(reg.pc); reg.status &= 0xCF; return 0; }
uint8_t RP6502::PLP() { pull(reg.status); reg.status |= 0x20; return 0; }
uint8_t RP6502::JSR() { push(--reg.pc); reg.pc = addr_abs; return 0; }
uint8_t RP6502::JMP() { reg.pc = addr_abs; return 0; }
uint8_t RP6502::RTS() { pull(reg.pc); reg.pc++; return 0; }
uint8_t RP6502::PHA() { push(reg.a); return 0; }
uint8_t RP6502::PLA() { pull(reg.a); setZN(reg.a); return 0; }
uint8_t RP6502::TXS() { reg.stkp = reg.x; return 0; }
uint8_t RP6502::CLC() { reg.status &= 0xFE; return 0; }
uint8_t RP6502::CLD() { reg.status &= 0xF7; return 0; }
uint8_t RP6502::CLI() { reg.status &= 0xFB; return 0; }
uint8_t RP6502::CLV() { reg.status &= 0xBF; return 0; }
uint8_t RP6502::SEC() { reg.status |= 0x01; return 0; }
uint8_t RP6502::SED() { reg.status |= 0x08; return 0; }
uint8_t RP6502::SEI() { reg.status |= 0x04; return 0; }
uint8_t RP6502::DEX() { setZN(--reg.x); return 0; }
uint8_t RP6502::DEY() { setZN(--reg.y); return 0; }
uint8_t RP6502::INX() { setZN(++reg.x); return 0; }
uint8_t RP6502::INY() { setZN(++reg.y); return 0; }
uint8_t RP6502::TAX() { setZN(reg.x = reg.a); return 0; }
uint8_t RP6502::TAY() { setZN(reg.y = reg.a); return 0; }
uint8_t RP6502::TSX() { setZN(reg.x = reg.stkp); return 0; }
uint8_t RP6502::TXA() { setZN(reg.a = reg.x); return 0; }
uint8_t RP6502::TYA() { setZN(reg.a = reg.y); return 0; }
uint8_t RP6502::LDA() { fetch(); setZN(reg.a = reg.m); return 1; }
uint8_t RP6502::ORA() { fetch(); setZN(reg.a|= reg.m); return 1; }
uint8_t RP6502::EOR() { fetch(); setZN(reg.a^= reg.m); return 1; }
uint8_t RP6502::AND() { fetch(); setZN(reg.a&= reg.m); return 1; }
uint8_t RP6502::LDX() { fetch(); setZN(reg.x = reg.m); return 1; }
uint8_t RP6502::LDY() { fetch(); setZN(reg.y = reg.m); return 1; }
uint8_t RP6502::CMP() { fetch(); SetFlag(C, reg.a>=reg.m); setZN(reg.m = reg.a-reg.m); return 1; }
uint8_t RP6502::CPX() { fetch(); SetFlag(C, reg.x>=reg.m); setZN(reg.m = reg.x-reg.m); return 0; }
uint8_t RP6502::CPY() { fetch(); SetFlag(C,reg. y>=reg.m); setZN(reg.m = reg.y-reg.m); return 0; }
uint8_t RP6502::ASL() { fetch(); SetFlag(C, reg.m&N); setZN(reg.m <<= 1); restore(); return 0; }
uint8_t RP6502::LSR() { fetch(); SetFlag(C, reg.m&C); setZN(reg.m >>= 1); restore(); return 0; }
uint8_t RP6502::ROL() { fetch(); SetFlag(C, reg.m&N); setZN(reg.m = (reg.m << 1) | (CF     )); restore(); return 0; }
uint8_t RP6502::ROR() { fetch(); SetFlag(C, reg.m&C); setZN(reg.m = (reg.m >> 1) | (CF << 7)); restore(); return 0; }
uint8_t RP6502::BIT() { fetch(); SetFlag(V, reg.m&V); SetFlag(Z, !(reg.a&reg.m)); SetFlag(N, reg.m&N);     return 0; }
uint8_t RP6502::DEC() { fetch(); bus->MemAccess(addr_abs, --reg.m, true); setZN(reg.m); return 0; }
uint8_t RP6502::INC() { fetch(); bus->MemAccess(addr_abs, ++reg.m, true); setZN(reg.m); return 0; }
uint8_t RP6502::STA() { bus->MemAccess(addr_abs, reg.a, true); return 0; }
uint8_t RP6502::STX() { bus->MemAccess(addr_abs, reg.x, true); return 0; }
uint8_t RP6502::STY() { bus->MemAccess(addr_abs, reg.y, true); return 0; }
// This illegal operations
uint8_t RP6502::AHX() { fetch(); bus->MemAccess(addr_abs, reg.a&reg.x&reg.m, true); return 0; }
uint8_t RP6502::SHX() { fetch(); bus->MemAccess(addr_abs, reg.x&reg.m, true); return 0; }
uint8_t RP6502::SAX() { bus->MemAccess(addr_abs, reg.a&reg.x, true); return 0; }
uint8_t RP6502::KIL() { bus->MemAccess(addr_abs, 0xFF, true); cycles++; return 0; }
uint8_t RP6502::TAS() { reg.status = reg.a&reg.x; return AHX(); }
uint8_t RP6502::LAS() { fetch(); reg.a = reg.x = reg.stkp = reg.m&reg.stkp; return 0; }
uint8_t RP6502::AXS() { fetch(); SetFlag(C, reg.a>=reg.m); setZN(reg.x &= reg.a-reg.m); return 1; }
uint8_t RP6502::SLO() { return ASL() & ORA(); }
uint8_t RP6502::ANC() { return AND() & opcode&0xF0?ROL():ASL(); } /* not write? */
uint8_t RP6502::RLA() { return ROL() & AND(); }
uint8_t RP6502::SRE() { return LSR() & EOR(); }
uint8_t RP6502::ALR() { return AND() & LSR(); }
uint8_t RP6502::RRA() { return ROR() & ADC(); }
uint8_t RP6502::ARR() { return AND() & ROR(); }
uint8_t RP6502::XAA() { return TXA() & AND(); }
uint8_t RP6502::LAX() { return LDA() & LDX(); }
uint8_t RP6502::DCP() { return DEC() & CMP(); }
uint8_t RP6502::ISC() { return INC() & SBC(); }
/*--------ILLEGAL END-----------*/
///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
bool RP6502::complete() { return cycles == 0; }