# nes_emulator
NES Emulator 
Based https://github.com/OneLoneCoder/olcNES
- Fix CPU. Optimized and add illegal opcodes.
- Fix Mappers. Add new mappers.
- Fix PPU.
- Fix APU. Work all 5 channels.

## Program Example
```C++
#include "nes/BUS.h"

uint32_t * screen_buffer = new uint32_t[320*240];
BUS * NES = new NES();
CARTRIDGE * cart = new CARTRIDGE("file.nes");

if (!cart->ImageValid()) return false;
NES->ConnectCartridge(cart);
NES->PPU.setFrameBuffer(screen_buffer);
NES->PPU.setScale(320, 2 /*Screen size, 0-pixelPerfect 1-4x3 2-320*/);
NES->Reset();

while(true){ //main loop
    NES->CPU.controller[0] = SomeReadControler(1); /*readControler1 function return byte status buttons*/
    NES->CPU.controller[1] = SomeReadControler(2); /*readControler2 function return byte status buttons*/
    uint8_t SkeepBuffer = (int)((1789773.0/SoundSamplesPerSec)  * ((double) currentFPS / 59.0));
    do { 
        NES->Clock();
        if (NES->APU.getAudioReady(SkeepBuffer)) {
            playSample(NES->APU.sample);
        }
    } while (!NES->PPU.frame_complete);
    NES->PPU.frame_complete = false;
    render(screen_buffer);
}
```
