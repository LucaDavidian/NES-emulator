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
	void Clock() { bus.Clock(); }
	bool FrameComplete() { return ppu.FrameComplete(); }
	Screen &GetScreen() { return screen; }
	int16_t GetAudioSample() { return apu.GetAudioOutput(); }
	void Reset() { bus.Reset(); }
	void InsertCartridge(const char *name) { cartridge = new Cartridge; cartridge->LoadRom(name); bus.InsertCartridge(cartridge); }
	void RemoveCartridge() { delete cartridge; }
private:
	Bus bus;
	CPU cpu;
	PPU ppu;
	APU apu;
	Screen screen;	
	Controller controller;
	Cartridge *cartridge;
};

#endif  // NES_H