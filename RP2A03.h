#pragma once

#include <string>
#include <map>
#include <vector>

class BUS;

class RP2A03
{
public:
	RP2A03();
	~RP2A03();

	bool DMC_IRQ = false;
	const uint8_t  LengthCounters[32]	= { 10,254,20, 2,40, 4,80, 6,160, 8,60,10,14,12,26,14,12, 16,24,18,48,20,96,22,192,24,72,26,16,28,32,30 };
	const uint16_t DMCperiods[16]		= { 428,380,340,320,286,254,226,214,190,160,142,128,106,84,72,54 };
	const uint16_t NoisePeriods[16] 		= { 2,4,8,16,32,48,64,80,101,127,190,254,381,508,1017,2034 };
	uint8_t MemAccess(uint16_t addr, uint8_t data = 0x00, bool write = false);
	short sample;
	bool CPU_IRQ = false;	
    bool Frame_IRQ = false;
	void reset();
	void clock();
	bool getAudioReady(unsigned SkeepBuffer);
	void ConnectBus(BUS *n) { bus = n; }
private:
	struct channel {
		uint8_t reg0;
		uint8_t reg1;
		uint8_t reg2;
		uint8_t reg3;

		int length_counter;
		int linear_counter;
		int address;
		int envelope;
		int sweep_delay;
		int env_delay;
		int level;
		int wave_counter;
		int phase;
		int hold;
		int WaveLength;

	} chn[5];
	BUS * bus = nullptr;
	int16_t direct_output;
	bool ChannelsEnabled[5] = { false };
	bool FiveCycleDivider = false;
	bool IRQdisable = false;
	struct { int16_t lo, hi; } hz240counter = { 0,0 };
	bool count(int& v, int reset);
	int tick(uint8_t c);
};
