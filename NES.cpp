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

	/**** iNES mapper 000 games ****/
	//cartridge->LoadRom("ROMs/nestest.nes");
	//cartridge->LoadRom("ROMs/Ice Climber.nes");	
	//cartridge->LoadRom("ROMs/Super Mario Bros.nes");
	//cartridge->LoadRom("ROMs/Donkey Kong.nes");
	//cartridge->LoadRom("ROMs/Donkey Kong 3.nes");

	/**** iNES mapper 001 games ****/
	//cartridge->LoadRom("ROMs/Bubble Bobble.nes");
	//cartridge->LoadRom("ROMs/The Legend of Zelda.nes");	
	//cartridge->LoadRom("ROMs/Zelda II.nes");
	//cartridge->LoadRom("ROMs/Tetris.nes");
	//cartridge->LoadRom("ROMs/Darkwing Duck.nes");
	//cartridge->LoadRom("ROMs/Double Dragon.nes");
	//cartridge->LoadRom("ROMs/Mega Man II.nes");

	/**** iNES mapper 002 games ****/
	cartridge->LoadRom("ROMs/DuckTales.nes");
	//cartridge->LoadRom("ROMs/DuckTales 2.nes");
	//cartridge->LoadRom("ROMs/Castlevania.nes");

	/**** iNES mapper 003 games ****/
	//cartridge->LoadRom("ROMs/Arkanoid.nes");

	/**** iNES mapper 004 games ****/
	//cartridge->LoadRom("ROMs/Bubble Bobble Part 2.nes");
	//cartridge->LoadRom("ROMs/Double Dragon II.nes");
	//cartridge->LoadRom("ROMs/Super Mario Bros 3.nes");

	bus.InsertCartridge(cartridge);

	bus.Reset();
}

void NES::Clock() 
{ 
	bus.Clock(); 
}