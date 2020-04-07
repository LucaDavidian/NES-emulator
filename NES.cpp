#include "NES.hpp"
#include "screen.hpp"

NES::NES() : cpu(bus), cartridge(nullptr)
{
	bus.ConnectCPU(&cpu);
	bus.ConnectPPU(&ppu);
	bus.ConnectAPU(&apu);
	ppu.ConnectScreen(&screen);
	bus.ConnectController(&controller);
}
