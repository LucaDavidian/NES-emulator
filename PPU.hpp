#ifndef PPU_H
#define PPU_H

#include <cstdint>

class Cartridge;
class Screen;

class PPU
{
public:
	PPU();

	void ConnectScreen(Screen *screen) { this->screen = screen; }
	void ConnectCartridge(Cartridge *cartridge) { this->cartridge = cartridge; }

	bool FrameComplete() { if (frameComplete) { frameComplete = false; return true; } else return false; }

	void DisplayPatternTable(uint8_t index);
	void DisplayBackground();

	void Reset();
	void Clock();

	bool InterruptAsserted() const { return interruptAsserted; }
	void AcknowledgeInterrupt() { interruptAsserted = false; }

	uint8_t ReadRegister(uint16_t registerAddress);
	void WriteRegister(uint16_t registerAddress, uint8_t data);
private:
	bool interruptAsserted = false;
private:
	uint8_t Read(uint16_t address);
	void Write(uint16_t address, uint8_t data);	
private:
	uint8_t VRAM[2][1024];      // 2 KiB VRAM (nametables)
	uint8_t palettes[32];       // 32 bytes palette memory

	struct Sprite
	{
		uint8_t y;
		uint8_t tileID;
		uint8_t attribute;
		uint8_t x;
	};
	Sprite OAM[64];            // 256 bytes internal OAM - object attribute memory (64 x 4-byte sprites)
	Sprite secondaryOAM[8];    // 32 bytes internal secondary OAM (8 x 4-byte sprites)
	Cartridge *cartridge;
	Screen *screen;
private:
	struct Color
	{
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	} paletteColors[0x40];
private:
	/**** PPU registers ****/
	union
	{
		struct
		{
			uint8_t baseNametableAddressX : 1;   // bit 10 of nametable base address
			uint8_t baseNametableAddressY : 1;   // bit 11 of nametable base address
			uint8_t addressIncrement : 1;        // read/write address increment - 0: increment by 1, 1: increment by 32
			uint8_t spritePatternTable : 1;      // sprite pattern table base address - 0: 0x0000, 1: 0x1000 (ignored in 8x16 mode)
			uint8_t backgroundPatternTable : 1;  // background pattern table base address - 0: 0x0000, 1: 0x1000
			uint8_t spriteSize : 1;              // sprite size - 0: 8x8px, 1: 8x16px
			uint8_t masterSlaveSelect : 1; 
			uint8_t interruptEnable : 1;         // generate interrupt at start of vblank interval - 0: disabled, 1: enabled
		} bits;
		uint8_t reg;
	} control;             // 0x2000: PPU control register (write-only)

	union
	{
		struct
		{
			uint8_t greyscale : 1;             // 0: normal, 1: grayscale
			uint8_t showBackgroundLeft : 1;    
			uint8_t showSpritesLeft : 1;
			uint8_t renderBackground : 1;      // render background - 0: off, 1: on    
			uint8_t renderSprites : 1;         // render sprites - 0: off, 1: on (clearing both bits 3 and 4 disables rendering)
			uint8_t emphasizeRed : 1;
			uint8_t emphasizeGreen : 1;
			uint8_t emphasizeBlue : 1;
		} bits;
		uint8_t reg;
	} mask;                // 0x2001: PPU mask register (write-only)
	 
	union
	{
		struct
		{
			uint8_t unused : 5;
			uint8_t spriteOverflow : 1;
			uint8_t spriteZeroHit : 1;    // opaque pixel of background overlaps opaque pixel of sprite 0 (cleared @ cycle 1 of pre-render scanline 261)
			uint8_t verticalBlank : 1;    // vertical blanking period has started (set @ cycle 1 of scanline 241, cleared @ cycle 1 of pre-render scanline 261) - 0: not in vblank, 1: in vblank
		} bits;
		uint8_t reg;
	} status;              // 0x2002: PPU status register (read-only)

	/**** internal PPU address registers ****/
	union
	{
		struct
		{
			uint16_t coarseX : 5;
			uint16_t coarseY : 5;
			uint16_t baseNametableAddressX : 1;
			uint16_t baseNametableAddressY : 1;
			uint16_t fineY : 3;
			uint16_t unused : 1;
		} bits;
		uint16_t reg;
	} addressT, addressV;   // temporary VRAM adddress register and VRAM address register

	uint8_t fineX;
	bool latched;

	uint8_t dataBuffer;

	uint8_t oamAddress;
private:
	int16_t cycle;
	int16_t scanline;
	bool frameComplete;

	/**** background rendering ****/
	void IncrementAddressX();
	void IncrementAddressY();
	void CopyAddressX();
	void CopyAddressY();
	void LoadBackgroundShiftRegisters();
	void ShiftBackgroundRegisters();

	uint8_t tileID;                              // fetched background tile
	uint8_t attribute;                           // fetched background nametable byte
	uint8_t backgroundLow;                       // fetched background pattern table byte (low bit plane)
	uint8_t backgroundHigh;                      // fetched background pattern table byte (high bit plane)

	uint16_t backgroundShiftRegisterLow;         // 2x 16-bits shift registers
	uint16_t backgroundShiftRegisterHigh;

	uint8_t attributeLatchLow;                   // 2x 8-bits latches
	uint8_t attributeLatchHigh;
	uint8_t attributeShiftRegisterLow;           // 2x 8-bits shift register
	uint8_t attributeShiftRegisterHigh;

	/**** sprite rendering ****/	
	uint8_t OAMEntry;                            // currently evaluated sprite - sprites are ordered by decreasing priority
	uint8_t spriteCount;                         // number of sprites in secondary OAM (max 8) 
	uint8_t numScanlineSprites;                  // number of sprites to be rendered on current scanline (max 8)
	bool allSpritesEvaluated;
	bool spriteZero, s0;

	uint8_t spriteTileID;
	uint8_t spriteY;
	uint8_t spriteTileY;
	uint16_t spriteTileBaseAddress;

	uint8_t spriteShiftRegisterLow[8];
	uint8_t spriteShiftRegisterHigh[8];
	uint8_t spriteAttribute[8];
	int8_t spriteXCounter[8];
};

#endif  // PPU_H
