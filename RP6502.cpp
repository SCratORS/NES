#include "RP6502.h"
#include "BUS.h"

#define Implied (lookup[opcode].addrmode == &RP6502::IMP)

// Constructor
RP6502::RP6502()
{
	using a = RP6502;
	lookup = 
	{	/*      0                   1                   2                   3                   4                   5                   6                   7                   8                   9                   A                   B                   C                     D                 E                     F*/
/*0*/	{&a::BRK,&a::IMP,7},{&a::ORA,&a::IZX,6},{&a::KIL,&a::IMP,2},{&a::SLO,&a::IZX,8},{&a::NOP,&a::ZP0,3},{&a::ORA,&a::ZP0,3},{&a::ASL,&a::ZP0,5},{&a::SLO,&a::ZP0,5},{&a::PHP,&a::IMP,3},{&a::ORA,&a::IMM,2},{&a::ASL,&a::IMP,2},{&a::ANC,&a::IMM,2},{&a::NOP,&a::ABS,4},{&a::ORA,&a::ABS,4},{&a::ASL,&a::ABS,6},{&a::SLO, &a::ABS,6},
/*1*/	{&a::BPL,&a::REL,2},{&a::ORA,&a::IZY,5},{&a::KIL,&a::IMP,2},{&a::SLO,&a::IZY,8},{&a::NOP,&a::ZPX,4},{&a::ORA,&a::ZPX,4},{&a::ASL,&a::ZPX,6},{&a::SLO,&a::ZPX,6},{&a::CLC,&a::IMP,2},{&a::ORA,&a::ABY,4},{&a::NOP,&a::IMP,2},{&a::SLO,&a::ABY,7},{&a::NOP,&a::ABX,4},{&a::ORA,&a::ABX,4},{&a::ASL,&a::ABX,7},{&a::SLO, &a::ABX,7},
/*2*/	{&a::JSR,&a::ABS,6},{&a::AND,&a::IZX,6},{&a::KIL,&a::IMP,2},{&a::RLA,&a::IZX,8},{&a::BIT,&a::ZP0,3},{&a::AND,&a::ZP0,3},{&a::ROL,&a::ZP0,5},{&a::RLA,&a::ZP0,5},{&a::PLP,&a::IMP,4},{&a::AND,&a::IMM,2},{&a::ROL,&a::IMP,2},{&a::ANC,&a::IMM,2},{&a::BIT,&a::ABS,4},{&a::AND,&a::ABS,4},{&a::ROL,&a::ABS,6},{&a::RLA, &a::ABS,6},
/*3*/	{&a::BMI,&a::REL,2},{&a::AND,&a::IZY,5},{&a::KIL,&a::IMP,2},{&a::RLA,&a::IZY,8},{&a::NOP,&a::ZPX,4},{&a::AND,&a::ZPX,4},{&a::ROL,&a::ZPX,6},{&a::RLA,&a::ZPX,6},{&a::SEC,&a::IMP,2},{&a::AND,&a::ABY,4},{&a::NOP,&a::IMP,2},{&a::RLA,&a::ABY,7},{&a::NOP,&a::ABX,4},{&a::AND,&a::ABX,4},{&a::ROL,&a::ABX,7},{&a::RLA, &a::ABX,7},
/*4*/	{&a::RTI,&a::IMP,6},{&a::EOR,&a::IZX,6},{&a::KIL,&a::IMP,2},{&a::SRE,&a::IZX,8},{&a::NOP,&a::ZP0,3},{&a::EOR,&a::ZP0,3},{&a::LSR,&a::ZP0,5},{&a::SRE,&a::ZP0,5},{&a::PHA,&a::IMP,3},{&a::EOR,&a::IMM,2},{&a::LSR,&a::IMP,2},{&a::ALR,&a::IMM,2},{&a::JMP,&a::ABS,3},{&a::EOR,&a::ABS,4},{&a::LSR,&a::ABS,6},{&a::SRE, &a::ABS,6},
/*5*/	{&a::BVC,&a::REL,2},{&a::EOR,&a::IZY,5},{&a::KIL,&a::IMP,2},{&a::SRE,&a::IZY,8},{&a::NOP,&a::ZPX,4},{&a::EOR,&a::ZPX,4},{&a::LSR,&a::ZPX,6},{&a::SRE,&a::ZPX,6},{&a::CLI,&a::IMP,2},{&a::EOR,&a::ABY,4},{&a::NOP,&a::IMP,2},{&a::SRE,&a::ABY,7},{&a::NOP,&a::ABX,4},{&a::EOR,&a::ABX,4},{&a::LSR,&a::ABX,7},{&a::SRE, &a::ABX,7},
/*6*/	{&a::RTS,&a::IMP,6},{&a::ADC,&a::IZX,6},{&a::KIL,&a::IMP,2},{&a::RRA,&a::IZX,8},{&a::NOP,&a::ZP0,3},{&a::ADC,&a::ZP0,3},{&a::ROR,&a::ZP0,5},{&a::RRA,&a::ZP0,5},{&a::PLA,&a::IMP,4},{&a::ADC,&a::IMM,2},{&a::ROR,&a::IMP,2},{&a::ARR,&a::IMM,2},{&a::JMP,&a::IND,5},{&a::ADC,&a::ABS,4},{&a::ROR,&a::ABS,6},{&a::RRA, &a::ABS,6},
/*7*/	{&a::BVS,&a::REL,2},{&a::ADC,&a::IZY,5},{&a::KIL,&a::IMP,2},{&a::RRA,&a::IZY,8},{&a::NOP,&a::ZPX,4},{&a::ADC,&a::ZPX,4},{&a::ROR,&a::ZPX,6},{&a::RRA,&a::ZPX,6},{&a::SEI,&a::IMP,2},{&a::ADC,&a::ABY,4},{&a::NOP,&a::IMP,2},{&a::RRA,&a::ABY,7},{&a::NOP,&a::ABX,4},{&a::ADC,&a::ABX,4},{&a::ROR,&a::ABX,7},{&a::RRA, &a::ABX,7},
/*8*/	{&a::NOP,&a::IMM,2},{&a::STA,&a::IZX,6},{&a::NOP,&a::IMM,2},{&a::SAX,&a::IZX,6},{&a::STY,&a::ZP0,3},{&a::STA,&a::ZP0,3},{&a::STX,&a::ZP0,3},{&a::SAX,&a::ZP0,3},{&a::DEY,&a::IMP,2},{&a::NOP,&a::IMM,2},{&a::TXA,&a::IMP,2},{&a::XAA,&a::IMM,2},{&a::STY,&a::ABS,4},{&a::STA,&a::ABS,4},{&a::STX,&a::ABS,4},{&a::SAX, &a::ABS,4},
/*9*/	{&a::BCC,&a::REL,2},{&a::STA,&a::IZY,6},{&a::KIL,&a::IMP,2},{&a::AHX,&a::IZY,6},{&a::STY,&a::ZPX,4},{&a::STA,&a::ZPX,4},{&a::STX,&a::ZPY,4},{&a::SAX,&a::ZPY,4},{&a::TYA,&a::IMP,2},{&a::STA,&a::ABY,5},{&a::TXS,&a::IMP,2},{&a::TAS,&a::ABY,5},{&a::NOP,&a::ABX,5},{&a::STA,&a::ABX,5},{&a::SHX,&a::ABY,5},{&a::AHX, &a::ABY,5},
/*A*/	{&a::LDY,&a::IMM,2},{&a::LDA,&a::IZX,6},{&a::LDX,&a::IMM,2},{&a::LAX,&a::IZX,6},{&a::LDY,&a::ZP0,3},{&a::LDA,&a::ZP0,3},{&a::LDX,&a::ZP0,3},{&a::LAX,&a::ZP0,3},{&a::TAY,&a::IMP,2},{&a::LDA,&a::IMM,2},{&a::TAX,&a::IMP,2},{&a::LAX,&a::IMM,2},{&a::LDY,&a::ABS,4},{&a::LDA,&a::ABS,4},{&a::LDX,&a::ABS,4},{&a::LAX, &a::ABS,4},
/*B*/	{&a::BCS,&a::REL,2},{&a::LDA,&a::IZY,5},{&a::KIL,&a::IMP,2},{&a::LAX,&a::IZY,5},{&a::LDY,&a::ZPX,4},{&a::LDA,&a::ZPX,4},{&a::LDX,&a::ZPY,4},{&a::LAX,&a::ZPY,4},{&a::CLV,&a::IMP,2},{&a::LDA,&a::ABY,4},{&a::TSX,&a::IMP,2},{&a::LAS,&a::ABY,4},{&a::LDY,&a::ABX,4},{&a::LDA,&a::ABX,4},{&a::LDX,&a::ABY,4},{&a::LAX, &a::ABY,4},
/*C*/	{&a::CPY,&a::IMM,2},{&a::CMP,&a::IZX,6},{&a::NOP,&a::IMM,2},{&a::DCP,&a::IZX,8},{&a::CPY,&a::ZP0,3},{&a::CMP,&a::ZP0,3},{&a::DEC,&a::ZP0,5},{&a::DCP,&a::ZP0,5},{&a::INY,&a::IMP,2},{&a::CMP,&a::IMM,2},{&a::DEX,&a::IMP,2},{&a::AXS,&a::IMM,2},{&a::CPY,&a::ABS,4},{&a::CMP,&a::ABS,4},{&a::DEC,&a::ABS,6},{&a::DCP, &a::ABS,6},
/*D*/	{&a::BNE,&a::REL,2},{&a::CMP,&a::IZY,5},{&a::KIL,&a::IMP,2},{&a::DCP,&a::IZY,8},{&a::NOP,&a::ZPX,4},{&a::CMP,&a::ZPX,4},{&a::DEC,&a::ZPX,6},{&a::DCP,&a::ZPX,6},{&a::CLD,&a::IMP,2},{&a::CMP,&a::ABY,4},{&a::NOP,&a::IMP,2},{&a::DCP,&a::ABY,7},{&a::NOP,&a::ABX,4},{&a::CMP,&a::ABX,4},{&a::DEC,&a::ABX,7},{&a::DCP, &a::ABX,7},
/*E*/	{&a::CPX,&a::IMM,2},{&a::SBC,&a::IZX,6},{&a::NOP,&a::IMM,2},{&a::ISC,&a::IZX,8},{&a::CPX,&a::ZP0,3},{&a::SBC,&a::ZP0,3},{&a::INC,&a::ZP0,5},{&a::ISC,&a::ZP0,5},{&a::INX,&a::IMP,2},{&a::SBC,&a::IMM,2},{&a::NOP,&a::IMP,2},{&a::SBC,&a::IMM,2},{&a::CPX,&a::ABS,4},{&a::SBC,&a::ABS,4},{&a::INC,&a::ABS,6},{&a::ISC, &a::ABS,6},
/*F*/	{&a::BEQ,&a::REL,2},{&a::SBC,&a::IZY,5},{&a::KIL,&a::IMP,2},{&a::ISC,&a::IZY,8},{&a::NOP,&a::ZPX,4},{&a::SBC,&a::ZPX,4},{&a::INC,&a::ZPX,6},{&a::ISC,&a::ZPX,6},{&a::SED,&a::IMP,2},{&a::SBC,&a::ABY,4},{&a::NOP,&a::IMP,2},{&a::ISC,&a::ABY,7},{&a::NOP,&a::ABX,4},{&a::SBC,&a::ABX,4},{&a::INC,&a::ABX,7},{&a::ISC, &a::ABX,7},
	};
}

RP6502::~RP6502(){}

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

void RP6502::branch() {	addr_abs = pc + addr_rel;	cycles += crosspage(addr_abs, pc)?2:1;	pc = addr_abs; }
void RP6502::setZN(uint8_t v) { SetFlag(Z, !v ); SetFlag(N, v&N); }
void RP6502::restore()	{ if (Implied) a = m;	else bus->MemAccess(addr_abs, m, true);}
uint8_t RP6502::fetch()	{	if (!Implied) m = bus->MemAccess(addr_abs);	return m; }
uint8_t  RP6502::pull(uint8_t  &v)	{	v = bus->MemAccess(0x0100 + ++stkp); return v; }
uint16_t RP6502::pull(uint16_t &v)	{ v = bus->MemAccess(0x0100 + ++stkp) | bus->MemAccess(0x0100 + ++stkp) << 8;	return v; }
void RP6502::push(uint8_t  v) {	bus->MemAccess(0x0100 + stkp--, v, true); }
void RP6502::push(uint16_t v) {	bus->MemAccess(0x0100 + stkp--, (v >> 8) & 0x00FF, true);	bus->MemAccess(0x0100 + stkp--, v & 0x00FF, true); }
uint8_t RP6502::ab_(uint8_t v){ ZP0(); uint16_t addr_tmp = bus->MemAccess(pc++)<<8;	addr_abs |= addr_tmp;	addr_abs += v;	return crosspage(addr_abs, addr_tmp)?1:0; }
uint8_t RP6502::zp_(uint8_t v){	ZP0(); addr_abs += v; addr_abs &= 0x00FF; return 0; }

void RP6502::reset() {
	a = x = y = m = 0x00;
	status = 0x00 | U;
	stkp = 0xFD;
	pc = bus->MemAccess(0xFFFC) | (bus->MemAccess(0xFFFD) << 8);
	addr_rel = addr_abs = 0x0000;
	cycles = 8;
	dma_page = 0x00; dma_addr = 0x00; dma_data = 0x00;
	dma_dummy = true;
	dma_transfer = false;
}

void RP6502::irq() {
	if (GetFlag(I)) return;
	status &= 0xFB; /* B = 0; U = 1; I = 1; */
	push(pc);
	push(status);
	addr_abs = 0xFFFE;
	pc = (bus->MemAccess(addr_abs + 1) << 8) | bus->MemAccess(addr_abs + 0);
	cycles = 12;
}


void RP6502::nmi() {
	status &= 0xFB; /* B = 0; U = 1; I = 1; */
	push(pc);
	push(status);
	addr_abs = 0xFFFA;
	pc = (bus->MemAccess(addr_abs + 1) << 8) | bus->MemAccess(addr_abs + 0);
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
		opcode = bus->MemAccess(pc++);
		status |= 0x20;
		cycles = lookup[opcode].cycles;
		cycles += ((this->*lookup[opcode].addrmode)() & (this->*lookup[opcode].operate)());
		status |= 0x20;
	}
	clock_count++;
	cycles--;
}

uint8_t RP6502::GetFlag(FLAGS6502 f) {
	return ((status & f) > 0) ? 1 : 0;
}

void RP6502::SetFlag(FLAGS6502 f, bool v) {
	if (f==C) CF = GetFlag(C);
	if (v) 	status |= f;
	else 	status &= ~f;
}

uint8_t RP6502::IND() {
	uint16_t addr_tmp  =  bus->MemAccess(pc++) | bus->MemAccess(pc++) << 8;
	if ((addr_tmp & 0xFF) == 0xFF) 	addr_abs  = bus->MemAccess(addr_tmp & 0xFF00) << 8;
	else														addr_abs  = bus->MemAccess(addr_tmp + 0x0001) << 8;
																addr_abs |= bus->MemAccess(addr_tmp + 0x0000);
	return 0;
}
uint8_t RP6502::IZX() {
	uint16_t addr_tmp = bus->MemAccess(pc++);
	addr_abs  = bus->MemAccess((uint16_t)(addr_tmp + (uint16_t)x + 0) & 0x00FF);
	addr_abs |= bus->MemAccess((uint16_t)(addr_tmp + (uint16_t)x + 1) & 0x00FF) << 8;
	return 0;
}
uint8_t RP6502::IZY() {
	uint16_t addr_tmp = bus->MemAccess(pc++);
	addr_abs = bus->MemAccess((addr_tmp + 0) & 0x00FF);
	addr_tmp = bus->MemAccess((addr_tmp + 1) & 0x00FF) << 8;
	addr_abs |= addr_tmp;
	addr_abs += y;
	return crosspage(addr_abs, addr_tmp)?1:0;
}
uint8_t RP6502::ABX() { return ab_(x); }
uint8_t RP6502::ABY() { return ab_(y); }
uint8_t RP6502::ZPX() { return zp_(x); }
uint8_t RP6502::ZPY() { return zp_(y); }
uint8_t RP6502::IMP() { m = a; return 0; }
uint8_t RP6502::IMM() { addr_abs = pc++; return 0; }
uint8_t RP6502::ZP0() { addr_abs = bus->MemAccess(pc++); return 0; }
uint8_t RP6502::ABS() { ZP0(); addr_abs |= bus->MemAccess(pc++)<<8; return 0; }
uint8_t RP6502::REL() { addr_rel = bus->MemAccess(pc++); if (addr_rel&N) addr_rel |= 0xFF00; return 0; }
/* OPCODES */
uint8_t RP6502::ADC() { fetch(); temp = a + m + GetFlag(C); SetFlag(C, temp & 0xFF00); SetFlag(V, (~(m^a)&(temp^a)) & N); setZN(a = temp); return 1; }
uint8_t RP6502::SBC() { fetch(); uint16_t v = m ^ 0x00FF; temp = a + v + GetFlag(C); SetFlag(C, temp & 0xFF00); SetFlag(V, (temp^a)&(temp^v) & N); setZN(a = temp); return 1; }
uint8_t RP6502::BRK() { SetFlag(I, 1); push(++pc); push((uint8_t) (status | B | U)); pc = bus->MemAccess(0xFFFE) | (bus->MemAccess(0xFFFF) << 8); return 0; }
uint8_t RP6502::NOP() { return opcode&0x1C?1:0; }
uint8_t RP6502::BCC() { if (!GetFlag(C)) branch(); return 0; }
uint8_t RP6502::BCS() { if ( GetFlag(C)) branch(); return 0; }
uint8_t RP6502::BNE() { if (!GetFlag(Z)) branch(); return 0; }
uint8_t RP6502::BEQ() { if ( GetFlag(Z)) branch(); return 0; }
uint8_t RP6502::BPL() { if (!GetFlag(N)) branch(); return 0; }
uint8_t RP6502::BMI() { if ( GetFlag(N)) branch(); return 0; }
uint8_t RP6502::BVC() { if (!GetFlag(V)) branch(); return 0; }
uint8_t RP6502::BVS() { if ( GetFlag(V)) branch(); return 0; }
uint8_t RP6502::PHP() { push((uint8_t) (status | B | U)); status &= 0xCF; return 0; }
uint8_t RP6502::RTI() { pull(status); pull(pc); status &= 0xCF; return 0; }
uint8_t RP6502::PLP() { pull(status); status |= 0x20; return 0; }
uint8_t RP6502::JSR() { push(--pc); pc = addr_abs; return 0; }
uint8_t RP6502::JMP() { pc = addr_abs; return 0; }
uint8_t RP6502::RTS() { pull(pc); pc++; return 0; }
uint8_t RP6502::PHA() { push(a); return 0; }
uint8_t RP6502::PLA() { pull(a); setZN(a); return 0; }
uint8_t RP6502::TXS() { stkp = x; return 0; }
uint8_t RP6502::CLC() { status &= 0xFE; return 0; }
uint8_t RP6502::CLD() { status &= 0xF7; return 0; }
uint8_t RP6502::CLI() { status &= 0xFB; return 0; }
uint8_t RP6502::CLV() { status &= 0xBF; return 0; }
uint8_t RP6502::SEC() { status |= 0x01; return 0; }
uint8_t RP6502::SED() { status |= 0x08; return 0; }
uint8_t RP6502::SEI() { status |= 0x04; return 0; }
uint8_t RP6502::DEX() { setZN(--x); return 0; }
uint8_t RP6502::DEY() { setZN(--y); return 0; }
uint8_t RP6502::INX() { setZN(++x); return 0; }
uint8_t RP6502::INY() { setZN(++y); return 0; }
uint8_t RP6502::TAX() { setZN(x = a); return 0; }
uint8_t RP6502::TAY() { setZN(y = a); return 0; }
uint8_t RP6502::TSX() { setZN(x = stkp); return 0; }
uint8_t RP6502::TXA() { setZN(a = x); return 0; }
uint8_t RP6502::TYA() { setZN(a = y); return 0; }
uint8_t RP6502::LDA() { fetch(); setZN(a = m); return 1; }
uint8_t RP6502::ORA() { fetch(); setZN(a|= m); return 1; }
uint8_t RP6502::EOR() { fetch(); setZN(a^= m); return 1; }
uint8_t RP6502::AND() { fetch(); setZN(a&= m); return 1; }
uint8_t RP6502::LDX() { fetch(); setZN(x = m); return 1; }
uint8_t RP6502::LDY() { fetch(); setZN(y = m); return 1; }
uint8_t RP6502::CMP() { fetch(); SetFlag(C, a>=m); setZN(m = a-m); return 1; }
uint8_t RP6502::CPX() { fetch(); SetFlag(C, x>=m); setZN(m = x-m); return 0; }
uint8_t RP6502::CPY() { fetch(); SetFlag(C, y>=m); setZN(m = y-m); return 0; }
uint8_t RP6502::ASL() { fetch(); SetFlag(C, m&N); setZN(m <<= 1); restore(); return 0; }
uint8_t RP6502::LSR() { fetch(); SetFlag(C, m&C); setZN(m >>= 1); restore(); return 0; }
uint8_t RP6502::ROL() { fetch(); SetFlag(C, m&N); setZN(m = (m << 1) | (CF     )); restore(); return 0; }
uint8_t RP6502::ROR() { fetch(); SetFlag(C, m&C); setZN(m = (m >> 1) | (CF << 7)); restore(); return 0; }
uint8_t RP6502::BIT() { fetch(); SetFlag(V, m&V); SetFlag(Z, !(a&m)); SetFlag(N, m&N);     return 0; }
uint8_t RP6502::DEC() { fetch(); bus->MemAccess(addr_abs, --m, true); setZN(m); return 0; }
uint8_t RP6502::INC() { fetch(); bus->MemAccess(addr_abs, ++m, true); setZN(m); return 0; }
uint8_t RP6502::STA() { bus->MemAccess(addr_abs, a, true); return 0; }
uint8_t RP6502::STX() { bus->MemAccess(addr_abs, x, true); return 0; }
uint8_t RP6502::STY() { bus->MemAccess(addr_abs, y, true); return 0; }
// This illegal operations
uint8_t RP6502::AHX() { fetch(); bus->MemAccess(addr_abs, a&x&m, true); return 0; }
uint8_t RP6502::SHX() { fetch(); bus->MemAccess(addr_abs, x&m, true); return 0; }
uint8_t RP6502::SAX() { bus->MemAccess(addr_abs, a&x, true); return 0; }
uint8_t RP6502::KIL() { bus->MemAccess(addr_abs, 0xFF, true); cycles++; return 0; }
uint8_t RP6502::TAS() { status = a&x; return AHX(); }
uint8_t RP6502::LAS() { fetch(); a = x = stkp = m&stkp; return 0; }
uint8_t RP6502::AXS() { fetch(); SetFlag(C, a>=m); setZN(x &= a-m); return 1; }
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