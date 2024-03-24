#pragma once
#include <cstring>
#include "CARTRIDGE.h"

#define MAX_SPRITE 64

class BUS;

class RP2C02
{
private:
	const uint32_t palScreen[0x40] = {
	    0x747474, 0x24188C, 0x0000A8, 0x44009C, 0x8C0074, 0xA80010, 0xA40000, 0x7C0800,
	    0x402C00, 0x004400, 0x005000, 0x003C14, 0x183C5C, 0x000000, 0x000000, 0x000000,
	    0xBCBCBC, 0x0070EC, 0x2038EC, 0x8000F0, 0xBC00BC, 0xE40058, 0xD82800, 0xC84C0C,
	    0x887000, 0x009400, 0x00A800, 0x009038, 0x008088, 0x000000, 0x000000, 0x000000,
	    0xFCFCFC, 0x3CBCFC, 0x5C94FC, 0xCC88FC, 0xF478FC, 0xFC74B4, 0xFC7460, 0xFC9838,
	    0xF0BC3C, 0x80D010, 0x4CDC48, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
	    0xFCFCFC, 0xA8E4FC, 0xC4D4FC, 0xD4C8FC, 0xFCC4FC, 0xFCC4D8, 0xFCBCB0, 0xFCD8A8,
	    0xFCE4A0, 0xE0FCA0, 0xA8F0BC, 0xB0FCCC, 0x9CFCF0, 0xC4C4C4, 0x000000, 0x000000
	};

public:
	RP2C02();
	~RP2C02() {}
	uint8_t	 MemAccess(uint16_t addr, uint8_t data = 0x00, bool write = false);
	void ConnectBus(BUS *n) { bus = n; }
	void ConnectCartridge(CARTRIDGE * cartridge) {CART = cartridge;}
	void reset();
	void clock();
	bool nmi = false;
	uint8_t* pOAM = (uint8_t*)OAM;
	bool frame_complete = false;
	uint32_t* GetFrameBuffer() { return FrameBuffer; }
	uint8_t	 PPUMemAccess(uint16_t addr, uint8_t data = 0x00, bool write = false);
private:
	CARTRIDGE * CART= nullptr;
	uint32_t * FrameBuffer;
	BUS     *bus = nullptr;
	void rendering_pixel();
	void rendering_tick();

	union PPUSTATUS	{
		struct {
			uint8_t open_bus : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};	uint8_t reg;
	} status;
	union PPUMASK {
		struct {
			uint8_t grayscale : 1;				//0x01
			uint8_t render_background_left : 1;	//0x02
			uint8_t render_sprites_left : 1;	//0x04
			uint8_t render_background : 1;	//0x08
			uint8_t render_sprites : 1;	//0x10
			uint8_t enhance_red : 1;	//0x20
			uint8_t enhance_green : 1;	//0x40
			uint8_t enhance_blue : 1; // 0x80
		};	uint8_t reg;
	} mask;
	union PPUCTRL {
		struct {
			uint8_t nametable_x : 1;
			uint8_t nametable_y : 1;
			uint8_t increment_mode : 1;
			uint8_t pattern_sprite : 1;
			uint8_t pattern_background : 1;
			uint8_t sprite_size : 1;
			uint8_t slave_mode : 1; // unused
			uint8_t enable_nmi : 1;
		};	uint8_t reg;
	} control;
	union PPUSCROLL {
		struct {
			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};	uint16_t reg = 0x0000;
	} tram_addr, vram_addr;

	struct sObjectAttributeEntry {
		uint8_t y;			// Y position of sprite
		uint8_t id;			// ID of tile from pattern memory
		uint8_t attribute;	// Flags define how sprite should be rendered
		uint8_t x;			// X position of sprite
	} OAM[64], spriteScanline[MAX_SPRITE];
	uint8_t	tblName[2][0x400];
	uint8_t	tblPalette[32];

	uint8_t oam_addr = 0x00;
	uint8_t fine_x = 0x00;
	bool address_latch = false;
	uint8_t ppu_data_buffer = 0x00;
	int16_t scanline = 0;
	int16_t cycle = 0;
	bool odd_frame = false;
	uint16_t scanline_end = 341;
	// Background rendering
	uint8_t bg_next_tile_attrib = 0x00;
	uint16_t bg_next_tile_addr = 0x0000;
	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo  = 0x0000;
	uint16_t bg_shifter_attrib_hi  = 0x0000;

	// Sprites rendering
	uint8_t sprite_count;
	uint8_t sprite_shifter_pattern_lo[MAX_SPRITE];
	uint8_t sprite_shifter_pattern_hi[MAX_SPRITE];
	// Sprite Zero Collision Flags
	bool bSpriteZeroHitPossible = false;
	bool bSpriteZeroBeingRendered = false;

};