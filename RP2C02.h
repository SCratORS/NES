#pragma once
#include <cstring>
#include <cmath>
#include "CARTRIDGE.h"

#define MAX_SPRITE 64

class BUS;

class RP2C02
{
private:
	const uint32_t palScreen[0x40] = {
		/*
		0x808080, 0x0000BB, 0x3700BF, 0x8400A6, 0xBB006A, 0xB7001E, 0xB30000, 0x912600,
		0x7B2B00, 0x003E00, 0x00480D, 0x003C22, 0x002F66, 0x000000, 0x000000, 0x000000,
		0xC8C8C8, 0x0059FF, 0x443CFF, 0xB733CC, 0xFF33AA, 0xFF375E, 0xFF371A, 0xD54B00,
		0xC46200, 0x3C7B00, 0x1E8415, 0x009566, 0x0084C4, 0x111111, 0x000000, 0x000000,
		0xFFFFFF, 0x0095FF, 0x6F84FF, 0xD56FFF, 0xFF77CC, 0xFF6F99, 0xFF7B59, 0xFF915F,
		0xFFA233, 0xA6BF00, 0x51D96A, 0x4DD5AE, 0x00D9FF, 0x666666, 0x000000, 0x000000,
		0xFFFFFF, 0x84BFFF, 0xBBBBFF, 0xD0BBFF, 0xFFBFEA, 0xFFBFCC, 0xFFC4B7, 0xFFCCAE,
		0xFFD9A2, 0xCCE199, 0xAEEEB7, 0xAAF7EE, 0xB3EEFF, 0xDDDDDD, 0x000000, 0x000000
		*/
		0x808080, 0x0023bb, 0x2f23bf, 0x7223a6, 0xa1236a, 0x9e231e, 0x9a2300, 0x7d4400,
		0x6a4800, 0x005800, 0x00610d, 0x005722, 0x004c66, 0x000000, 0x000000, 0x000000,
		0xC8C8C8, 0x0070ff, 0x3b57ff, 0x9e4fcc, 0xdc4faa, 0xdc525e, 0xdc521a, 0xb86400,
		0xa97800, 0x348d00, 0x1a9515, 0x00a466, 0x0095c4, 0x111111, 0x000000, 0x000000,
		0xFFFFFF, 0x00a4ff, 0x6095ff, 0xb883ff, 0xdc8acc, 0xdc8399, 0xdc8d59, 0xdca05f,
		0xdcaf33, 0x8fc800, 0x46de6a, 0x42dbae, 0x00deff, 0x666666, 0x000000, 0x000000,
		0xFFFFFF, 0x72c8ff, 0xa1c4ff, 0xb3c4ff, 0xdcc8ea, 0xdcc8cc, 0xdcccb7, 0xdcd3ae,
		0xdcdea2, 0xb0e599, 0x96f0b7, 0x93f8ee, 0x9af0ff, 0xDDDDDD, 0x000000, 0x000000
	};
	CARTRIDGE * CART = nullptr;
	uint32_t * FrameBuffer = nullptr;
	BUS     *bus = nullptr;
	void rendering_pixel();
	void rendering_tick();
	struct sObjectAttributeEntry {
		uint8_t y;			// Y position of sprite
		uint8_t id;			// ID of tile from pattern memory
		uint8_t attribute;	// Flags define how sprite should be rendered
		uint8_t x;			// X position of sprite
	};

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
	sObjectAttributeEntry OAM[MAX_SPRITE], spriteScanline[MAX_SPRITE];
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
	uint8_t scale = 0;
	uint8_t scale_offset = 32;
	uint16_t screen_width = 256;

public:
	RP2C02();
	~RP2C02() {}
	uint8_t	 MemAccess(uint16_t addr, uint8_t data = 0x00, bool write = false);
	void ConnectBus(BUS *n) { bus = n; }
	void ConnectCartridge(CARTRIDGE * cartridge) {CART = cartridge;}
	void RejectCartridge() {CART = nullptr;}
	void reset();
	void clock();
	bool nmi = false;
	uint8_t* pOAM = (uint8_t*)OAM;
	bool frame_complete = false;
	void setFrameBuffer(uint32_t * ScreenBuffer) {FrameBuffer = ScreenBuffer;}
	uint8_t	 PPUMemAccess(uint16_t addr, uint8_t data = 0x00, bool write = false);
	void setScale(uint16_t screen_width, uint8_t scale);
};