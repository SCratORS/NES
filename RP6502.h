#pragma once
#include <string>
#include <map>
#include <vector>

class BUS;

class RP6502
{
public:
	RP6502();
	~RP6502();
public:
	uint8_t  m      = 0x00;
	uint8_t  a      = 0x00;
	uint8_t  x      = 0x00;
	uint8_t  y      = 0x00;
	uint8_t  stkp   = 0x00;
	uint16_t pc     = 0x0000;
	uint8_t  status = 0x00;
	uint8_t  MemAccess(uint16_t addr, uint8_t data = 0x00, bool write = false);
	uint8_t  controller[4];

	void reset();
	void irq();
	void nmi();
	void dma_clock(uint32_t counter);
	void clock();
	bool complete();

	void ConnectBus(BUS *n) { bus = n; }

	std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nStop);

	enum FLAGS6502
	{
		C = (1 << 0),	// Carry Bit 				x1
		Z = (1 << 1),	// Zero					x2	
		I = (1 << 2),	// Disable Interrupts			x4
		D = (1 << 3),	// Decimal Mode (unused in this implementation)	x8
		B = (1 << 4),	// Break					x16	
		U = (1 << 5),	// Unused					x32
		V = (1 << 6),	// Overflow					x
		N = (1 << 7),	// Negative
	};

private:
	BUS * bus = nullptr;

	uint8_t GetFlag(FLAGS6502 f);
	void    SetFlag(FLAGS6502 f, bool v);
	uint8_t CF;
	//helper function
	void setZN(uint8_t v);
	uint8_t fetch();
	bool	crosspage(uint16_t addr_page1, uint16_t addr_page2) {return (addr_page1 & 0xFF00) != (addr_page2 & 0xFF00);}
	void 	branch();
	void 	restore();
	uint8_t pull(uint8_t& v);
	uint16_t pull(uint16_t& v);
	void push(uint8_t v);
	void push(uint16_t v);
	uint8_t ab_(uint8_t v);
	uint8_t zp_(uint8_t v);

	uint16_t temp        = 0x0000; 
	uint16_t addr_abs    = 0x0000;
	uint16_t addr_rel    = 0x00; 
	uint8_t  opcode      = 0x00;
	uint8_t  cycles      = 0;
	uint32_t clock_count = 0;
	uint8_t dma_page = 0x00;
	uint8_t dma_addr = 0x00;
	uint8_t dma_data = 0x00;
	bool dma_dummy = true;
	bool dma_transfer = false;
	uint32_t controller_state[2];

	struct INSTRUCTION
	{
		uint8_t     (RP6502::*operate )(void) = nullptr;
		uint8_t     (RP6502::*addrmode)(void) = nullptr;
		uint8_t     cycles = 0;
	};
	std::vector<INSTRUCTION> lookup;

private: 
	uint8_t IMP();	uint8_t IMM();	
	uint8_t ZP0();	uint8_t ZPX();	
	uint8_t ZPY();	uint8_t REL();
	uint8_t ABS();	uint8_t ABX();	
	uint8_t ABY();	uint8_t IND();	
	uint8_t IZX();	uint8_t IZY();

private: 
	/* Legal operation */
	uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
	uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
	uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
	uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
	uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
	uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
	uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
	uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
	uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
	uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
	uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
	uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
	uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
	uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();
	/* Illegal operation */
	uint8_t KIL();	uint8_t SLO();	uint8_t ANC();	uint8_t RLA();
	uint8_t SRE();	uint8_t ALR();	uint8_t RRA();	uint8_t ARR();
	uint8_t SAX();	uint8_t XAA();	uint8_t AHX();	uint8_t TAS();
	uint8_t SHX();	uint8_t LAX();	uint8_t LAS();	uint8_t DCP();
	uint8_t AXS();	uint8_t ISC();
};
