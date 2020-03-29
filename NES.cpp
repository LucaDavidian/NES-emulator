#include "NES.h"
#include "screen.hpp"

NES::NES() : cpu(bus), cartridge(nullptr)
{
	bus.ConnectCPU(&cpu);
	bus.ConnectPPU(&ppu);
	bus.ConnectAPU(&apu);
	ppu.ConnectScreen(&screen);
}

void NES::Initialize()
{
	controller = new Controller;
	bus.ConnectController(controller);

	Cartridge *cartridge = new Cartridge;

	/**** mapper 0 games ****/
	//cartridge->LoadRom("ROMs/nestest.nes");
	//cartridge->LoadRom("ROMs/Ice Climber.nes");	
	//cartridge->LoadRom("ROMs/Super Mario Bros.nes");
	//cartridge->LoadRom("ROMs/Donkey Kong.nes");
	//cartridge->LoadRom("ROMs/Donkey Kong 3.nes");

	/**** mapper 1 games ****/
	//cartridge->LoadRom("ROMs/Bubble Bobble.nes");
	cartridge->LoadRom("ROMs/The Legend of Zelda.nes");	
	//cartridge->LoadRom("ROMs/Zelda II.nes");

	/**** mapper 2 games ****/
	//cartridge->LoadRom("ROMs/Duck Tales.nes");

	/**** mapper 3 games ****/
	//cartridge->LoadRom("ROMs/Arkanoid.nes");
	
	bus.InsertCartridge(cartridge);

	bus.Reset();
}

void NES::Clock() 
{ 
	bus.Clock(); 
}