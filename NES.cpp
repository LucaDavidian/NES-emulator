#include "NES.h"
#include "screen.hpp"
#include <map>

std::map<uint16_t, std::string> m;

NES::NES() : cpu(bus), cartridge(nullptr)
{
	bus.ConnectCPU(&cpu);
	bus.ConnectPPU(&ppu);
	ppu.ConnectScreen(&screen);
}

void NES::Initialize()
{
	controller = new Controller;
	bus.ConnectController(controller);

	Cartridge *cartridge = new Cartridge;
	cartridge->LoadRom("ROMs/Super Mario Bros.nes");
	//cartridge->LoadRom("ROMs/Donkey Kong.nes");
	//cartridge->LoadRom("ROMs/Ice Climber.nes");
	//cartridge->LoadRom("ROMs/nestest.nes");
	bus.InsertCartridge(cartridge);

	cpu.Reset();   // TODO: bus reset!
	ppu.Reset();

	m = cpu.Disassemble(0x8000, 0xFFFF);
}