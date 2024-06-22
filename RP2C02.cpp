#include "RP2C02.h"
#include "BUS.h"

RP2C02::RP2C02() {}

void RP2C02::setScale(uint16_t screen_width, uint8_t scale) {
	this->scale = scale;
	this->screen_width = screen_width;
	uint16_t screen_mod = 256;
	switch (scale) {
		case 0: screen_mod += screen_mod * 0.11f; break;// 4:3 = 284
		case 1: screen_mod += screen_mod * 0.11f; break;// 4:3 = 284
		case 2: break;//PP = 256
		case 3: screen_mod += screen_mod * 0.205f; break;//GW = 308
		case 4: screen_mod += screen_mod * 0.25f; break;//320 = 320
	}
	this->scale_offset = (screen_width - screen_mod) >> 1; 
}

void RP2C02::SaveState(PPUState * state) {
	memcpy(state, &reg, sizeof(PPUState));
}

void RP2C02::LoadState(PPUState * state) {
	memcpy(&reg, state, sizeof(PPUState));
}

uint8_t	 RP2C02::MemAccess(uint16_t addr, uint8_t data, bool write) {
	uint8_t result = 0x00;
	switch (addr & 0x07) {
		case 0x00:	if (!write) break;
					reg.control.reg = data;
					reg.tram_addr.nametable_x = reg.control.nametable_x;
					reg.tram_addr.nametable_y = reg.control.nametable_y;
					break;
		case 0x01:	if (write) reg.mask.reg = data;	break;
		case 0x02:	if (write) break;
					result = (reg.status.reg & 0xE0) | (ppu_data_buffer & 0x1F);
					reg.status.vertical_blank = address_latch = 0;
					break;
		case 0x03:	if (write) oam_addr = data;	break;
		case 0x04:	if (write) pOAM[oam_addr] = data;
					else result =  pOAM[oam_addr];
					break;
		case 0x05:	if (!write) break;
					if (address_latch = !address_latch) { fine_x = data & 0x07; reg.tram_addr.coarse_x = data >> 3; }
					else { reg.tram_addr.fine_y = data & 0x07; reg.tram_addr.coarse_y = data >> 3; }
					break;
		case 0x06:	if (!write) break;
					if (address_latch = !address_latch)	reg.tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (reg.tram_addr.reg & 0x00FF);
					else { reg.tram_addr.reg = (reg.tram_addr.reg & 0xFF00) | data; reg.vram_addr = reg.tram_addr; }
					break;
		case 0x07:	result = ppu_data_buffer;
					uint8_t t = PPUMemAccess(reg.vram_addr.reg, write?data:result, write);
					if (write) result = t;
					else {
						ppu_data_buffer = t;
						if (reg.vram_addr.reg >= 0x3F00) result = ppu_data_buffer = (ppu_data_buffer & 0xC0) | (t & 0x3F);
					};
					reg.vram_addr.reg += reg.control.increment_mode ? 0x20 : 0x01;
					break;
	}
	return result;
}

uint8_t RP2C02::PPUMemAccess(uint16_t addr, uint8_t data, bool write) {
	addr &= 0x3FFF;
	uint8_t result = data;
	if (addr >= 0x3F00) {	if(addr%4 == 0) addr &= 0x0F;
							uint8_t& M = reg.tblPalette[addr&0x1F];
							if (write) M = data; else return(M & (reg.mask.grayscale ? 0x30 : 0x3F));
						}
	else if (addr >= 0x2000) { addr &= 0x0FFF;
							if (CART) {
								uint8_t& M = reg.tblName[(CART->Mirror() >> ((addr>>10)&0x03))&0x01][addr & 0x03FF];
								if  (write) M = data; else return M; }
							}
	else if (CART) CART->PPUMemAccess(addr, result, write);
	return result;
}

void RP2C02::reset() {
	fine_x = 0x00;
	address_latch = odd_frame = false;
	ppu_data_buffer = 0x00;
	scanline = -1;
	cycle = 0;
	bg_next_tile_attrib = 0x00;
	bg_shifter_pattern_lo = bg_shifter_pattern_hi = 0x0000;
	bg_shifter_attrib_lo = bg_shifter_attrib_hi = 0x0000;
	reg.status.reg = reg.mask.reg = reg.control.reg = 0x00;
	reg.vram_addr.reg = reg.tram_addr.reg = 0x0000;
}

void RP2C02::rendering_tick() {
	bool render_SB = (reg.mask.render_background || reg.mask.render_sprites);
	uint8_t nOAMEntry = 0;
	if (cycle==001 && scanline < 0) {reg.status.reg = 0;}
	if (cycle==304 && scanline < 0 && reg.mask.render_background) reg.vram_addr.reg = reg.tram_addr.reg;
	if (cycle==339 && scanline < 0 && reg.mask.render_background && odd_frame) {odd_frame = cycle = scanline = 0;}
	if ((cycle > 1 && cycle < 258) || (cycle > 320 && cycle < 338)) {
		if (reg.mask.render_background) {
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<= 1;
			bg_shifter_attrib_lo <<= 1;
			bg_shifter_attrib_hi <<= 1;
		}
		if (reg.mask.render_sprites && cycle < 258) for (int i = 0; i < sprite_count; i++) {
			if (spriteScanline[i].x > 0) spriteScanline[i].x--;
			else {
				sprite_shifter_pattern_lo[i] <<= 1;
				sprite_shifter_pattern_hi[i] <<= 1;
			}
		}
		if ((cycle-1)%8 == 7 && render_SB && !(++reg.vram_addr.coarse_x)%31) reg.vram_addr.nametable_x = ~reg.vram_addr.nametable_x;
		if ((cycle-1)%8 == 0) {
			bg_shifter_pattern_lo |= PPUMemAccess(bg_next_tile_addr);
			bg_shifter_pattern_hi |= PPUMemAccess(bg_next_tile_addr + 8);
			bg_shifter_attrib_lo  = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
			bg_shifter_attrib_hi  = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
			bg_next_tile_attrib = PPUMemAccess(0x23C0 + (reg.vram_addr.reg&0xC00) + ((reg.vram_addr.coarse_y>>2)<<3) + (reg.vram_addr.coarse_x>>2));		
			if (reg.vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
			if (reg.vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
			bg_next_tile_attrib &= 0x03;
			bg_next_tile_addr = (reg.control.pattern_background << 12) + (PPUMemAccess(0x2000 | (reg.vram_addr.reg & 0xFFF)) << 4) + reg.vram_addr.fine_y;
		}
	}
	if (cycle == 257) {
		if (render_SB) {
			reg.vram_addr.nametable_x = reg.tram_addr.nametable_x;
			reg.vram_addr.coarse_x    = reg.tram_addr.coarse_x;
			if (!(++reg.vram_addr.fine_y)%7)
				if (reg.vram_addr.coarse_y == 29) {
					reg.vram_addr.coarse_y = 0;
					reg.vram_addr.nametable_y ^= 1;
				} else (++reg.vram_addr.coarse_y)%31;
		}
		if (scanline  < 0) return;
		std::memset(spriteScanline, 0xFF, MAX_SPRITE * sizeof(sObjectAttributeEntry));
		sprite_count = 0;
		bSpriteZeroHitPossible = false;
		while (nOAMEntry < 64 && sprite_count < MAX_SPRITE)	{
			int16_t diff = ((int16_t)scanline - (int16_t)reg.OAM[nOAMEntry].y);			
			if (diff >= 0 && diff < (reg.control.sprite_size ? 0x10 : 0x08)) {
				if (nOAMEntry == 0) bSpriteZeroHitPossible = true;
				memcpy(&spriteScanline[sprite_count], &reg.OAM[nOAMEntry], sizeof(sObjectAttributeEntry));	
				uint8_t a = !(((scanline - spriteScanline[sprite_count].y<8)>0)^((spriteScanline[sprite_count].attribute&0x80)>0));
				uint16_t sprite_pattern_addr = 
				  ((reg.control.sprite_size?(spriteScanline[sprite_count].id & 0x01):reg.control.pattern_sprite) << 12) |
				  ((reg.control.sprite_size?((spriteScanline[sprite_count].id & 0xFE) + a):spriteScanline[sprite_count].id) <<  4) |
				  (((scanline - spriteScanline[sprite_count].y)&(reg.control.sprite_size?0x07:0xFF))^
				  	((spriteScanline[sprite_count].attribute & 0x80)?0x07:0x00));
				sprite_shifter_pattern_lo[sprite_count] = PPUMemAccess(sprite_pattern_addr);
				sprite_shifter_pattern_hi[sprite_count] = PPUMemAccess(sprite_pattern_addr + 8);
				if (spriteScanline[sprite_count].attribute & 0x40) {
					auto flipbyte = [](uint8_t b)	{
						b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
						b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
						b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
						return b;
					};
					sprite_shifter_pattern_lo[sprite_count] = flipbyte(sprite_shifter_pattern_lo[sprite_count]);
					sprite_shifter_pattern_hi[sprite_count] = flipbyte(sprite_shifter_pattern_hi[sprite_count]);
				}
				sprite_count++;					
			}
			nOAMEntry++;
		}
		reg.status.sprite_overflow = (sprite_count > MAX_SPRITE);
	}
}

uint32_t avrColor(uint8_t a, uint8_t b, uint32_t c1, uint32_t c2) {
	uint8_t R = ((((c1 >> 16) & 0xFF) * a) + (((c2 >> 16) & 0xFF)*b)) / (a+b);
	uint8_t G = ((((c1 >> 8) & 0xFF) * a) + (((c2 >> 8) & 0xFF)*b)) / (a+b);
	uint8_t B = (((c1 & 0xFF) * a) + ((c2 & 0xFF)*b)) / (a+b);
	return (R << 16) | (G<<8) | B;
}


void RP2C02::rendering_pixel() {
	uint8_t pixel = 0x00, palette = 0x00;
	bool render_bg = reg.mask.render_background && (reg.mask.render_background_left || (cycle > 8));
	bool render_sp = reg.mask.render_sprites && (reg.mask.render_sprites_left || (cycle > 8));
	if (render_bg) {
			uint16_t bit_mux = 0x8000 >> fine_x;
			uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
			uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;
			uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
			uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
			pixel = (p1_pixel << 1) | p0_pixel;
			palette = pixel ? (bg_pal1 << 1) | bg_pal0 : 0x00; 
	}
	if (render_sp) {
		bSpriteZeroBeingRendered = false;
		for (uint8_t i = 0; i < sprite_count; i++) {
			if (spriteScanline[i].x == 0) {
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
				uint8_t fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;
				if (fg_pixel) {
					if (!(spriteScanline[i].attribute&0x20 && pixel)) {
						pixel = fg_pixel;
						palette = (spriteScanline[i].attribute & 0x03) + 0x04;
					}
					if (!i && bSpriteZeroHitPossible) bSpriteZeroBeingRendered = true;
					break;
				}
			}
		}
	}
	if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered && render_bg && render_sp) reg.status.sprite_zero_hit = 1;
	if (scanline < 0) return;
	if (scale_offset+cycle < 0 || scale_offset+cycle > screen_width) return;

	uint32_t offset = scanline * screen_width + scale_offset;
	uint32_t color = palScreen[PPUMemAccess(0x3F00+(palette<<2)+pixel)&0x3F];
	uint8_t mod = 0;
	switch (scale) {
		case 0: if (scanline%2) color = avrColor(3, 1, color, 0);
		case 1: mod = 9; break; // 4:3
		case 2: break;			//PixelPerfect
		case 3: mod = 5; break; //Stretch to Game&Watch
		case 4: mod = 4; break; //Stretch to 320
	}
	if (scale == 2) { //PixelPerfect
		FrameBuffer[offset + (cycle - 1)] = color; 
		return;
	}
	offset += (mod+1) * ((cycle - 1) / mod);
	uint8_t shift = ((cycle-1) % mod);
	FrameBuffer[offset + shift] = avrColor(shift, mod-shift, FrameBuffer[offset + shift] , color);
	if (mod%2 && (cycle - 1)==255) return;
	FrameBuffer[offset + shift+1] = color;
}

void RP2C02::clock() {
	if (scanline < 240) {
		rendering_tick();
		if (cycle > 0 && cycle < 257) rendering_pixel();
	}
	if (CART) CART->IRQScanline(cycle, scanline, reg.mask.reg, reg.control.reg);
	if (++cycle >= 341)	{
		cycle = 0;
		switch (++scanline) {
			case 240: 	reg.status.vertical_blank = 1; break;
			case 241:	if (reg.control.enable_nmi) nmi = true; break;
			case 261:	scanline = -1;
						frame_complete = odd_frame = true;
			break;
		}
	}
}