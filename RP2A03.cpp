#include "RP2A03.h"
#include "BUS.h"

RP2A03::RP2A03(){}

RP2A03::~RP2A03(){}

void RP2A03::SaveState(APUState * state) {
	memcpy(state, &reg, sizeof(APUState));
}

void RP2A03::LoadState(APUState * state) {
	memcpy(&reg, state, sizeof(APUState));
}


bool RP2A03::count(int& v, int reset) { return --v < 0 ? (v=reset),true : false; }

void RP2A03::reset() {
		CPU_IRQ = false;	
    Frame_IRQ = false;
    DMC_IRQ = false;
    reg.FiveCycleDivider = false;
    reg.IRQdisable = false;
    reg.hz240counter.lo = 0;
    reg.hz240counter.hi = 0;
    sample = 0;
    for (uint8_t i=0; i<5; i++) {
    	memset(&reg.chn[0]+i, 0x00, sizeof(channel) );
    	reg.ChannelsEnabled[i] = false;
    }

}

int RP2A03::tick(uint8_t c) {
	channel& ch = reg.chn[c];
	if (!reg.ChannelsEnabled[c]) return c<4?8:ch.linear_counter;//reg.output_direct_volume_dmc;
	int wl = (ch.WaveLength+1) << (c >= 2 ? 0 : 1);
	if(c == 3) wl = NoisePeriods[ ch.reg2&0x0F ] << 1;
	int volume = ch.length_counter ? ch.reg0&0x10 ? ch.reg0&0x0F : ch.envelope : 0;
	auto& S = ch.level;
	if(!count(ch.wave_counter, wl)) return S;
	switch(c) {
		default: if(wl < 8) return S = 8;
           		return S = (0x9F786040u & (1u << ((((ch.reg0>>6)&0x03)<<3) + ch.phase++ % 8))) ? volume : 0;
		case 2:	if(!ch.length_counter || !ch.linear_counter || wl < 3) return S = 8;
           		++ch.phase;
           		return S = (ch.phase & 15) ^ ((ch.phase & 16) ? 15 : 0);
    case 3:	if(!ch.hold) ch.hold = 1;
           		ch.hold = (ch.hold >> 1) | (((ch.hold ^ (ch.hold >> (ch.reg2&0x80 ? 6 : 1))) & 1) << 14);
           		return S = (ch.hold & 1) ? 0 : volume;
		case 4: if(ch.phase == 0) {
              if(!ch.length_counter && ch.reg3&0x40)	{
                  ch.length_counter = (ch.reg1 << 4) | 1;
                  ch.address = (ch.reg0 | 0x300) << 6;
              }
              ch.phase = 8;
              if(ch.length_counter > 0) {
                  ch.hold  = bus->MemAccess((uint16_t)(ch.address++) | 0x8000);
                  --ch.length_counter;
              } else {
              	reg.ChannelsEnabled[c] = ch.reg3&0x80 && (DMC_IRQ = true);
              }
            } 
            if(ch.phase > 0) ch.phase--;
            if (reg.ChannelsEnabled[c]) {
            	int v = ch.linear_counter;
            	if(ch.hold & (0x80 >> ch.phase)) v += 2; else v -= 2;
            	if(v >= 0x00 && v < 0x80) ch.linear_counter = v;
          	} 
          	return S = ch.linear_counter;	            
	}
}

uint8_t	 RP2A03::MemAccess(uint16_t addr, uint8_t data, bool write) {
	uint8_t result = data;
	channel& ch = reg.chn[(addr>>2) % 5];
	switch (addr<0x10 ? addr%4 : addr) {
		case 0: if (write) ch.reg0 = data; break;
		case 1: if (!write) break;
						ch.reg1 = data;
						ch.sweep_delay = (data>>4) & 0x07;
						break;
		case 2: if (!write) break;
						ch.reg2 = data;
						ch.WaveLength = (ch.WaveLength&0xFF00) | data;
						break;
		case 3: if (!write) break;
						ch.reg3 = data;
						ch.WaveLength =  ((ch.WaveLength&0x00FF) | (ch.reg3 << 8)) & 0x07FF;
						if(reg.ChannelsEnabled[addr>>2]) ch.length_counter = LengthCounters[ch.reg3>>3];
						ch.linear_counter = ch.reg0&0x7F;
						ch.env_delay      = ch.reg0&0x0F;
						ch.envelope       = 15;
						if(addr < 8) ch.phase = 0;
						break;
		case 0x10: if (!write) break;
						ch.reg3 = data;
						ch.WaveLength = DMCperiods[data&0x0F];
						break;
		case 0x11: if (write) reg.output_direct_volume_dmc = data&0x7F; break;
		case 0x12: if (write) ch.reg0 = data; ch.address = (ch.reg0 | 0x300) << 6; break;
		case 0x13: if (!write) break;
								ch.reg1 = data;
								ch.length_counter = (ch.reg1 << 4) | 1;
								break;
		case 0x15:	if (write) {
								DMC_IRQ = false;
								for(uint8_t c=0; c<5; c++) {
									reg.ChannelsEnabled[c] = data & (1 << c);
									if(!reg.ChannelsEnabled[c]) reg.chn[c].length_counter = 0;
									else if(c == 4 && reg.chn[c].length_counter == 0)
									reg.chn[c].length_counter = (reg.chn[c].reg1 << 4) | 1;
								}
							} else {
								result = 0x00;
								for(uint8_t c=0; c<5; c++) result |= (reg.chn[c].length_counter ? 1 << c : 0);
								if(Frame_IRQ) result |= 0x40; Frame_IRQ = false;
								if(DMC_IRQ)	result |= 0x80;
							}
							break;
		break;
		case 0x17: if (!write) break;
								reg.FiveCycleDivider = data & 0x80;
								reg.IRQdisable       = data & 0x40;
								reg.hz240counter     = { 0,0 };
								if(reg.IRQdisable) Frame_IRQ = DMC_IRQ = false;
								break;
	}
	return result;
}

bool RP2A03::getAudioReady(unsigned SkeepBuffer) {
	return ((reg.hz240counter.lo % SkeepBuffer) < 2);
}

void RP2A03::clock() {
	if((reg.hz240counter.lo += 2) >= 14915) {
		reg.hz240counter.lo -= 14915;
		if(++reg.hz240counter.hi >= 4+reg.FiveCycleDivider) reg.hz240counter.hi = 0;
	  if(!reg.IRQdisable && !reg.FiveCycleDivider && reg.hz240counter.hi==0) Frame_IRQ = true;
	  bool HalfTick = (reg.hz240counter.hi&5)==1, FullTick = reg.hz240counter.hi < 4;
	  for(uint8_t c=0; c<4; c++) {
	  	channel& ch = reg.chn[c];
	  	int wl = ch.WaveLength;
	    if(HalfTick && ch.length_counter && !(c==2 ? ch.reg0&0x80 : ch.reg0&0x20)) ch.length_counter -= 1;
			if(HalfTick && c < 2 && count(ch.sweep_delay, (ch.reg1>>4)&0x07 ))
				if(wl >= 8 && ch.reg1&0x80 && ch.reg1&0x07) {
					int s = wl >> (ch.reg1&0x07), d[4] = {s, s, ~s, -s};
					wl += d[(((ch.reg1>>3)&1) <<1) + c];
					if(wl < 0x800) { ch.WaveLength = wl;	}
				}
			if(FullTick && c == 2) ch.linear_counter = ch.reg0&0x80 ? ch.reg0&0x7F : (ch.linear_counter > 0 ? ch.linear_counter - 1 : 0);
			if(FullTick && c != 2 && count(ch.env_delay, ch.reg0&0x0F))
				if(ch.envelope > 0 || ch.reg0&0x20) ch.envelope = (ch.envelope-1) & 15;
		}
	}
	int16_t s[5];
	for(uint8_t c=0; c<5; c++) s[c] = tick(c);
  auto v = [](float m,float n, float d) { return n!=0.f ? m/n : d; };
  sample = 30000 *
    (v(95.88f,  (100.f + v(8128.f, s[0] + s[1], -100.f)), 0.f)
  +  v(159.79f, (100.f + v(1.0, s[2]/8227.f + s[3]/12241.f + s[4]/22638.f + reg.output_direct_volume_dmc/22638.f, -100.f)), 0.f)
    - 0.5f
    );	
}