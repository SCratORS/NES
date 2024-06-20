#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <fstream>

#include "Mapper_000.h"
#include "Mapper_001.h"
#include "Mapper_002.h"
#include "Mapper_003.h"
#include "Mapper_004.h"
#include "Mapper_007.h"
#include "Mapper_023.h"
#include "Mapper_066.h"
#include "Mapper_071.h"
#include "Mapper_212.h"


struct CARTState {
	uint32_t sizeMapState = 0;
	uint16_t maxCHRAddr = 0;
	uint16_t maxPRGAddr = 0;
	uint8_t * mapperState = nullptr;
	uint8_t * CHRam = nullptr;
	uint8_t * PRGRam = nullptr;
};

class CARTRIDGE
{
public:
	CARTRIDGE(const std::string& sFileName);
	~CARTRIDGE();
	bool ImageValid();
	void reset();
	bool MemAccess(uint16_t addr, uint8_t &data, bool write = false);
	bool PPUMemAccess(uint16_t addr, uint8_t &data, bool write = false);
	void IRQScanline(int16_t x, int16_t y, uint8_t mask, uint8_t control);
	bool IRQState();
	void IRQClear();
	uint8_t Mirror();
	void SaveState(CARTState * state);
	void LoadState(CARTState * state);
private:

	bool bImageValid = false;
	uint32_t cCHRLength = 0;
	uint32_t cPRGLength = 0;
	uint8_t nMapperID = 0;
	uint8_t nPRGBanks = 0;
	uint8_t nCHRBanks = 0;
	Mapper * pMapper;
	uint8_t * vPRGMemory = nullptr;
	uint8_t * vCHRMemory = nullptr;
	uint8_t hw_mirror = 0x01; 	// HARDWARE: 																0x01
								// HORIZONTAL	: 0x0C00:[1], 0x0800:[1], 0x0400:[0], 0x0000:[0] = 0b1100 = 0x0C
								// VARTICAL		: 0x0C00:[1], 0x0800:[0], 0x0400:[1], 0x0000:[0] = 0b1010 = 0x0A
								// ONESCREEN_HI	: 0x0C00:[1], 0x0800:[1], 0x0400:[1], 0x0000:[1] = 0b1111 = 0x0F
								// ONESCREEN_LO	: 0x0C00:[0], 0x0800:[0], 0x0400:[0], 0x0000:[0] = 0b0000 = 0x00
	CARTState reg;
	
};