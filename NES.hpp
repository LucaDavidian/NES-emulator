#ifndef NES_H
#define NES_H

#include "bus.hpp"
#include "CPU.hpp"
#include "PPU.hpp"
#include "APU.hpp"
#include "cartridge.hpp"
#include "screen.hpp"
#include "controller.hpp"

class NES
{
public:
	NES();
	void Initialize(); 
	void Clock();
	bool FrameComplete() { return ppu.FrameComplete(); }
	Screen &GetScreen() { return screen; }
	int16_t GetAudioSample() { return apu.GetAudioOutput(); }
private:
	Bus bus;
	CPU cpu;
	PPU ppu;
	APU apu;
	Screen screen;	
	Cartridge *cartridge;
	Controller *controller;
};

#endif  // NES_H