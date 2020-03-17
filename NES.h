#ifndef NES_H
#define NES_H

#include "bus.hpp"
#include "CPU.hpp"
#include "PPU.hpp"
#include "cartridge.hpp"
#include "screen.hpp"
#include "controller.h"

class NES
{
private:
	static const std::size_t screenWidth = 256;
	static const std::size_t screenHeight = 240;
public:
	NES();
	void Initialize(); 
	void Clock() { bus.Clock(); }
	bool FrameComplete() { return ppu.FrameComplete(); }
	Screen &GetScreen() { return screen; }
private:
	Bus bus;
	CPU cpu;
	PPU ppu;
	Screen screen;	
	Cartridge *cartridge;
	Controller *controller;
};

#endif  // NES_H