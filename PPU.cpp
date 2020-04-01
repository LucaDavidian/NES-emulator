#include "PPU.hpp"
#include "cartridge.hpp"
#include "screen.hpp"

PPU::PPU()
{
	paletteColors[0x00] = { 84, 84, 84 };
	paletteColors[0x01] = { 0, 30, 116 };
	paletteColors[0x02] = { 8, 16, 144 };
	paletteColors[0x03] = { 48, 0, 136 };
	paletteColors[0x04] = { 68, 0, 100 };
	paletteColors[0x05] = { 92, 0, 48 };
	paletteColors[0x06] = { 84, 4, 0 };
	paletteColors[0x07] = { 60, 24, 0 };
	paletteColors[0x08] = { 32, 42, 0 };
	paletteColors[0x09] = { 8, 58, 0 };
	paletteColors[0x0A] = { 0, 64, 0 };
	paletteColors[0x0B] = { 0, 60, 0 };
	paletteColors[0x0C] = { 0, 50, 60 };
	paletteColors[0x0D] = { 0, 0, 0 };
	paletteColors[0x0E] = { 0, 0, 0 };
	paletteColors[0x0F] = { 0, 0, 0 };

	paletteColors[0x10] = { 152, 150, 152 };
	paletteColors[0x11] = { 8, 76, 196 };
	paletteColors[0x12] = { 48, 50, 236 };
	paletteColors[0x13] = { 92, 30, 228 };
	paletteColors[0x14] = { 136, 20, 176 };
	paletteColors[0x15] = { 160, 20, 100 };
	paletteColors[0x16] = { 152, 34, 32 };
	paletteColors[0x17] = { 120, 60, 0 };
	paletteColors[0x18] = { 84, 90, 0 };
	paletteColors[0x19] = { 40, 114, 0 };
	paletteColors[0x1A] = { 8, 124, 0 };
	paletteColors[0x1B] = { 0, 118, 40 };
	paletteColors[0x1C] = { 0, 102, 120 };
	paletteColors[0x1D] = { 0, 0, 0 };
	paletteColors[0x1E] = { 0, 0, 0 };
	paletteColors[0x1F] = { 0, 0, 0 };

	paletteColors[0x20] = { 236, 238, 236 };
	paletteColors[0x21] = { 76, 154, 236 };
	paletteColors[0x22] = { 120, 124, 236 };
	paletteColors[0x23] = { 176, 98, 236 };
	paletteColors[0x24] = { 228, 84, 236 };
	paletteColors[0x25] = { 236, 88, 180 };
	paletteColors[0x26] = { 236, 106, 100 };
	paletteColors[0x27] = { 212, 136, 32 };
	paletteColors[0x28] = { 160, 170, 0 };
	paletteColors[0x29] = { 116, 196, 0 };
	paletteColors[0x2A] = { 76, 208, 32 };
	paletteColors[0x2B] = { 56, 204, 108 };
	paletteColors[0x2C] = { 56, 180, 204 };
	paletteColors[0x2D] = { 60, 60, 60 };
	paletteColors[0x2E] = { 0, 0, 0 };
	paletteColors[0x2F] = { 0, 0, 0 };

	paletteColors[0x30] = { 236, 238, 236 };
	paletteColors[0x31] = { 168, 204, 236 };
	paletteColors[0x32] = { 188, 188, 236 };
	paletteColors[0x33] = { 212, 178, 236 };
	paletteColors[0x34] = { 236, 174, 236 };
	paletteColors[0x35] = { 236, 174, 212 };
	paletteColors[0x36] = { 236, 180, 176 };
	paletteColors[0x37] = { 228, 196, 144 };
	paletteColors[0x38] = { 204, 210, 120 };
	paletteColors[0x39] = { 180, 222, 120 };
	paletteColors[0x3A] = { 168, 226, 144 };
	paletteColors[0x3B] = { 152, 226, 180 };
	paletteColors[0x3C] = { 160, 214, 228 };
	paletteColors[0x3D] = { 160, 162, 160 };
	paletteColors[0x3E] = { 0, 0, 0 };
	paletteColors[0x3F] = { 0, 0, 0 };
}

void PPU::Reset()
{
	controlRegister.reg = 0x00;
	maskRegister.reg = 0x00;
	statusRegister.reg = 0x00;

	addressV.reg = 0x0000;
	addressT.reg = 0x0000;
	fineX = 0x00;
	latched = false;
	dataBuffer = 0x00;

	cycle = 0;
	scanline = 0;

	tileID = 0x00;
	attribute = 0x00;
	backgroundLow = 0x00;
	backgroundHigh = 0x00;

	backgroundShiftRegisterLow = 0x0000;
	backgroundShiftRegisterHigh = 0x0000;
	attributeShiftRegisterLow = 0x0000;
	attributeShiftRegisterHigh = 0x0000;

	oamAddress = 0x00;
}

void PPU::IncrementAddressX()
{
	if (maskRegister.bits.renderBackground || maskRegister.bits.renderSprites)
	{
		if (addressV.bits.coarseX == 31)
		{
			addressV.bits.coarseX = 0;
			addressV.bits.baseNametableAddressX = addressV.bits.baseNametableAddressX == 0 ? 1 : 0;
		}
		else
			addressV.bits.coarseX++;
	}
}

void PPU::IncrementAddressY()
{
	if (maskRegister.bits.renderBackground || maskRegister.bits.renderSprites)
	{
		if (addressV.bits.fineY == 7)
		{
			addressV.bits.fineY = 0;

			if (addressV.bits.coarseY == 29)
			{
				addressV.bits.coarseY = 0;
				addressV.bits.baseNametableAddressY = addressV.bits.baseNametableAddressY == 0 ? 1 : 0;
			}
			else if (addressV.bits.coarseY == 31)
				addressV.bits.coarseY = 0;
			else
				addressV.bits.coarseY++;
		}
		else
			addressV.bits.fineY++;
	}
}

void PPU::CopyAddressX()
{
	if (maskRegister.bits.renderBackground || maskRegister.bits.renderSprites)
	{
		addressV.bits.coarseX = addressT.bits.coarseX;
		addressV.bits.baseNametableAddressX = addressT.bits.baseNametableAddressX;
	}
}

void PPU::CopyAddressY()
{
	if (maskRegister.bits.renderBackground || maskRegister.bits.renderSprites)
	{
		addressV.bits.coarseY = addressT.bits.coarseY;
		addressV.bits.baseNametableAddressY = addressT.bits.baseNametableAddressY;
		addressV.bits.fineY = addressT.bits.fineY;
	}
}

void PPU::LoadBackgroundShiftRegisters()
{
	backgroundShiftRegisterLow = backgroundShiftRegisterLow & 0xFF00 | backgroundLow;
	backgroundShiftRegisterHigh = backgroundShiftRegisterHigh & 0xFF00 | backgroundHigh;

	attributeShiftRegisterLow = attributeLatchLow;
	attributeShiftRegisterHigh = attributeLatchHigh;

	attributeLatchLow = attribute & 0x01 ? 0xFF : 0x00;
	attributeLatchHigh = attribute & 0x02 ? 0xFF : 0x00;
}

void PPU::ShiftBackgroundRegisters()
{
	static uint8_t shifted;

	if (maskRegister.bits.renderBackground)
	{
		backgroundShiftRegisterLow <<= 1;
		backgroundShiftRegisterHigh <<= 1;

		attributeShiftRegisterLow = attributeShiftRegisterLow << 1 | (attributeLatchLow & 0x80 >> shifted) >> 7 - shifted ;
		attributeShiftRegisterHigh = attributeShiftRegisterHigh << 1 | (attributeLatchHigh & 0x80 >> shifted) >> 7 - shifted;

		if (++shifted == 8)
			shifted = 0;
	}
}

void PPU::Clock()
{
	if (scanline >= 0 && scanline <= 239)              // visible frame
	{
		/**************** background rendering ********************/
		if (cycle >= 1 && cycle <= 256 || cycle >= 321 && cycle <= 336)
		{
			ShiftBackgroundRegisters();

			switch ((cycle - 1) % 8)
			{
				case 0:
					LoadBackgroundShiftRegisters();
					break;
				case 1:     // fetch background tile id from nametable byte
					tileID = Read(0x2000 | addressV.reg & 0x0FFF);
					break;
				case 3:     // fetch background attribute byte from attribute table and get palette 
					attribute = Read((0x2000 | addressV.bits.baseNametableAddressY << 11 | addressV.bits.baseNametableAddressX << 10) + 0x03C0 + (addressV.bits.coarseY >> 2 << 3) + (addressV.bits.coarseX >> 2));
					if (addressV.bits.coarseX % 4 / 2)
						attribute >>= 2;
					if (addressV.bits.coarseY % 4 / 2)
						attribute >>= 4;
					attribute &= 0x03;
					break;
				case 5:     // fetch background low byte from pattern table
					backgroundLow = Read(controlRegister.bits.backgroundPatternTable * 0x1000 + (tileID << 4) + addressV.bits.fineY);
					break;
				case 7:     // fetch background high byte from pattern table, increment X
					backgroundHigh = Read(controlRegister.bits.backgroundPatternTable * 0x1000 + (tileID << 4) + addressV.bits.fineY + 8);
					IncrementAddressX();
					break;
			}

			if (cycle == 256)
				IncrementAddressY();
		}
		else if (cycle == 257)
			CopyAddressX();
		else if (cycle == 338 || cycle == 340)
			tileID = Read(0x2000 | addressV.reg & 0x0FFF);	

		/******************** sprites rendering ********************/
		if (cycle == 0)
		{
			spriteCountScanline = spriteCountNextScanline;
			spriteCountNextScanline = 0;
			OAMEntry = 0;
			allSpritesEvaluated = false;
			spriteZeroOnScanline = spriteZeroNextScanline;
			spriteZeroNextScanline = false;
		}
		else if (cycle >= 1 && cycle <= 256)
		{
			if (maskRegister.bits.renderSprites)
				for (int i = 0; i < spriteCountScanline; i++)
					if (spriteXCounter[i] + 1)  // visible pixels start at cycle 1
						spriteXCounter[i]--;
					else
					{
						spriteShiftRegisterLow[i] <<= 1;
						spriteShiftRegisterHigh[i] <<= 1;
					}
			
			if (cycle >= 1 && cycle <= 64)           // clear secondary OAM
			{
				static uint8_t data;

				if (cycle % 2)    // odd PPU cycle
					data = 0xFF;                     // during cycles 1 - 64 reading from OAM memory returns 0xFF
				else              // even PPU cycle 
					((uint8_t*)secondaryOAM)[(cycle - 1) / 2] = data;
			}
			else if (cycle >= 65 && cycle <= 256)    // sprite evaluation
			{
				if (cycle % 2)  // odd PPU cycle
					spriteY = OAM[OAMEntry].y;
				else            // even PPU cycle
				{
					if (!allSpritesEvaluated && spriteCountNextScanline <= 8)
					{
						int16_t difference = (int16_t)scanline - (int16_t)spriteY; 

						if (spriteCountNextScanline < 8)
						{
							secondaryOAM[spriteCountNextScanline].y = spriteY;

							if (difference >= 0 && difference < (controlRegister.bits.spriteSize ? 16 : 8))
							{
								secondaryOAM[spriteCountNextScanline].tileID = OAM[OAMEntry].tileID;
								secondaryOAM[spriteCountNextScanline].attribute = OAM[OAMEntry].attribute;
								secondaryOAM[spriteCountNextScanline].x = OAM[OAMEntry].x;

								if (OAMEntry == 0)
									spriteZeroNextScanline = true;

								spriteCountNextScanline++;
							}
						}
						else if (spriteCountNextScanline == 8)
							if (difference >= 0 && difference < (controlRegister.bits.spriteSize ? 16 : 8))
							{
								statusRegister.bits.spriteOverflow = 1;
								spriteCountNextScanline++;
							}
					}

					OAMEntry++;

					if (OAMEntry == 64)
					{
						OAMEntry = 0;

						if (!allSpritesEvaluated)
							allSpritesEvaluated = true;
					}
				}
			}
		}
		else if (cycle >= 257 && cycle <= 320)    // sprite tile fetches - initialize internal sprite output units
		{
			switch ((cycle - 1) % 8)   // 8 cycles per sprite x 8 sprites total = 64 cycles
			{
				case 0:
					spriteY = secondaryOAM[cycle - 257 >> 3].y;					
					break;
				case 1:
					spriteTileID = secondaryOAM[cycle - 257 >> 3].tileID;
					break;
				case 2:
					spriteAttribute[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].attribute;
					break;
				case 3:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					break;
				case 4:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					break;
				case 5:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					spriteTileY = scanline - spriteY;  
					if (controlRegister.bits.spriteSize == 0)          // 8x8px sprite
					{
						if (spriteAttribute[cycle - 257 >> 3] & 0x80)  // vertical flip
							spriteTileY = 7 - spriteTileY;

						spriteTileBaseAddress = controlRegister.bits.spritePatternTable * 0x1000 + (spriteTileID << 4) + spriteTileY;
					}
					else                                               // 8x16px sprite
					{
						uint8_t patternTable = spriteTileID & 0x01;    // in 8x16px sprite mode bit 0 is the sprite pattern table
						spriteTileID &= ~0x01;                         // in 8x16px mode the 7 most significant bits are the ID of top tile (bottom tile is next tile ID)

						if (spriteAttribute[cycle - 257 >> 3] & 0x80)  // vertical flip
							spriteTileY = 15 - spriteTileY;

						if (spriteTileY > 7)
						{
							spriteTileID |= 0x01;                      
							spriteTileY -= 8;
						}	

						spriteTileBaseAddress = patternTable * 0x1000 + (spriteTileID  << 4) + spriteTileY;
					}
					spriteShiftRegisterLow[cycle - 257 >> 3] = Read(spriteTileBaseAddress);
					if (spriteAttribute[cycle - 257 >> 3] & 0x40)  // horizontal flip
					{
						uint8_t flip = spriteShiftRegisterLow[cycle - 257 >> 3];

						flip = (flip & 0xF0) >> 4 | (flip & 0x0F) << 4;
						flip = (flip & 0xCC) >> 2 | (flip & 0x33) << 2;
						flip = (flip & 0xAA) >> 1 | (flip & 0x55) << 1;

						spriteShiftRegisterLow[cycle - 257 >> 3] = flip;
					}
					break;
				case 6:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					break;
				case 7:				
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;	
					spriteShiftRegisterHigh[cycle - 257 >> 3] = Read(spriteTileBaseAddress + 8);
					if (spriteAttribute[cycle - 257 >> 3] & 0x40)  // horizontal flip
					{
						uint8_t flip = spriteShiftRegisterHigh[cycle - 257 >> 3];

						flip = (flip & 0xF0) >> 4 | (flip & 0x0F) << 4;
						flip = (flip & 0xCC) >> 2 | (flip & 0x33) << 2;
						flip = (flip & 0xAA) >> 1 | (flip & 0x55) << 1;

						spriteShiftRegisterHigh[cycle - 257 >> 3] = flip;
					}
					break;
			}		
		}
		//else if (cycle >= 321 && cycle <= 340 || cycle == 0)
		//	uint8_t garbage = secondaryOAM[0].y;
	}
	else if (scanline == 240)            // post-render scanline (do nothing)
		;
	else if (scanline == 241)            // scanline 241
	{
		if (cycle == 1)    // entered vertical blanking period, set status bit and emit interrupt (if enabled)
		{
			statusRegister.bits.verticalBlank = 1;

			if (controlRegister.bits.interruptEnable)
				interruptAsserted = true;
		}	
	}
	else if (scanline >= 242 && scanline <= 260)       // vertical blanking period
		;
	else if (scanline == 261)                          // pre-render scanline
	{
		/******************** background rendering ********************/
		if (cycle >= 1 && cycle <= 256 || cycle >= 321 && cycle <= 336)
		{
			if (cycle == 1)
			{
				statusRegister.bits.verticalBlank = 0;     // vertical blanking period ended - clear the status bit
				statusRegister.bits.spriteOverflow = 0;    // clear sprite overflow flag
				statusRegister.bits.spriteZeroHit = 0;     // clear sprite 0 hit flag
			}

			ShiftBackgroundRegisters();

			switch ((cycle - 1) % 8)
			{
				case 0:
					LoadBackgroundShiftRegisters();
					break;
				case 1:     // fetch background tile id from nametable byte
					tileID = Read(0x2000 | addressV.reg & 0x0FFF);
					break;
				case 3:     // fetch background attribute byte from attribute table and get palette 
					attribute = Read((0x2000 | addressV.bits.baseNametableAddressY << 11 | addressV.bits.baseNametableAddressX << 10) + 0x03C0 + (addressV.bits.coarseY >> 2 << 3) + (addressV.bits.coarseX >> 2));
					if (addressV.bits.coarseX % 4 / 2)
						attribute >>= 2;
					if (addressV.bits.coarseY % 4 / 2)
						attribute >>= 4;
					attribute &= 0x03;
					break;
				case 5:     // fetch background low byte from pattern table
					backgroundLow = Read(controlRegister.bits.backgroundPatternTable * 0x1000 + (tileID << 4) + addressV.bits.fineY);
					break;
				case 7:     // fetch background high byte from pattern table, increment X
					backgroundHigh = Read(controlRegister.bits.backgroundPatternTable * 0x1000 + (tileID << 4) + addressV.bits.fineY + 8);
					IncrementAddressX();
					break;
			}

			if (cycle == 256)
				IncrementAddressY();
		}
		else if (cycle == 257)
			CopyAddressX();
		else if (cycle >= 280 && scanline <= 304)
			CopyAddressY();
		else if (cycle == 338 || cycle == 340)
			tileID = Read(0x2000 | addressV.reg & 0x0FFF);

		/******************** sprites rendering ********************/
		if (cycle >= 1 && cycle <= 64)          // no secondary OAM clear on pre-render scanline 261
			;
		else if (cycle >= 65 && cycle <= 256)   // no sprite evaluation on pre-render scanline 261
			;
		else if (cycle >= 257 && cycle <= 320)  // sprite tile fetches - initialize internal sprite output units
		{
			switch ((cycle - 1) % 8)
			{
				case 0:
					spriteY = secondaryOAM[cycle - 257 >> 3].y;
					break;
				case 1:
					spriteTileID = secondaryOAM[cycle - 257 >> 3].tileID;
					break;
				case 2:
					spriteAttribute[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].attribute;
					break;
				case 3:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					break;
				case 4:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					break;
				case 5:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					spriteTileY = scanline - spriteY;
					if (controlRegister.bits.spriteSize == 0)    // 8x8px sprite
					{
						if (spriteAttribute[cycle - 257 >> 3] & 0x80)  // vertical flip
							spriteTileY = 7 - spriteTileY;
	
						spriteTileBaseAddress = controlRegister.bits.spritePatternTable * 0x1000 + (spriteTileID << 4) + spriteTileY;
					}
					else                                // 8x16px sprite
					{
						if (spriteAttribute[cycle - 257 >> 3] & 0x80)  // vertical flip
							spriteTileY = 15 - spriteTileY;
	
						if (spriteTileY > 7)
						{
							spriteTileID += 0x02;   // in 8x16px sprite mode bit 0 is the sprite pattern table
							spriteTileY -= 8;
						}
	
						spriteTileBaseAddress = (spriteTileID & 0x01) * 0x1000 + ((spriteTileID & 0xFE) << 3) + spriteTileY;
					}
					spriteShiftRegisterLow[cycle - 257 >> 3] = Read(spriteTileBaseAddress);
					if (spriteAttribute[cycle - 257 >> 3] & 0x40)  // horizontal flip
					{
						uint8_t flip = spriteShiftRegisterLow[cycle - 257 >> 3];

						flip = (flip & 0xF0) >> 4 | (flip & 0x0F) << 4;
						flip = (flip & 0xCC) >> 2 | (flip & 0x33) << 2;
						flip = (flip & 0xAA) >> 1 | (flip & 0x55) << 1;

						spriteShiftRegisterLow[cycle - 257 >> 3] = flip;
					}
					break;
				case 6:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					break;
				case 7:
					spriteXCounter[cycle - 257 >> 3] = secondaryOAM[cycle - 257 >> 3].x;
					spriteShiftRegisterHigh[cycle - 257 >> 3] = Read(spriteTileBaseAddress + 8);
					if (spriteAttribute[cycle - 257 >> 3] & 0x40)  // horizontal flip
					{
						uint8_t flip = spriteShiftRegisterHigh[cycle - 257 >> 3];

						flip = (flip & 0xF0) >> 4 | (flip & 0x0F) << 4;
						flip = (flip & 0xCC) >> 2 | (flip & 0x33) << 2;
						flip = (flip & 0xAA) >> 1 | (flip & 0x55) << 1;

						spriteShiftRegisterHigh[cycle - 257 >> 3] = flip;
					}
					break;
			}
		}
		//else if (cycle >= 321 && cycle <= 340 || cycle == 0)
		//	uint8_t garbage = secondaryOAM[0].y;
	}
	
	// render pixel
	uint8_t backgroundPixel = 0x00;
	uint8_t backgroundPalette = 0x00;

	if (maskRegister.bits.renderBackground)
	{
		uint16_t pixelMask = 0x8000 >> fineX;
		uint8_t paletteMask = 0x80 >> fineX;

		backgroundPixel |= (backgroundShiftRegisterLow & pixelMask) >> 15 - fineX;
		backgroundPixel |= (backgroundShiftRegisterHigh & pixelMask) >> 15 - fineX << 1 ;

		backgroundPalette |= (attributeShiftRegisterLow & paletteMask) >> 7 - fineX;
		backgroundPalette |= (attributeShiftRegisterHigh & paletteMask) >> 7 - fineX << 1;
	}

	uint8_t spritePixel = 0x00;
	uint8_t spritePalette = 0x00;
	uint8_t spritePriority = 0;
	
	if (maskRegister.bits.renderSprites)
	{
		for (int i = 0; i < spriteCountScanline; i++)
		{
			if (spriteXCounter[i] == -1)
			{
				spritePixel |= (spriteShiftRegisterLow[i] & 0x80) >> 7;
				spritePixel |= (spriteShiftRegisterHigh[i] & 0x80) >> 6;

				spritePalette = (spriteAttribute[i] & 0x03);
				spritePalette += 0x04;  // sprite palettes 

				spritePriority = spriteAttribute[i] & 0x20;
				
				if (spritePixel != 0)  // if sprite pixel is opaque exit loop, else keep looking for other sprites
				{
					if (spriteZeroOnScanline && i == 0)
						statusRegister.bits.spriteZeroHit = 1;

					break;
				}
			}
		}
	}

	uint8_t paletteIndex = 0x00;   // if backgroundPalette and spritePalette are 0x00 pixel is transparent
	
	if (backgroundPixel > 0 && spritePixel == 0)
		paletteIndex = backgroundPalette << 2 | backgroundPixel;
	else if (backgroundPixel == 0 && spritePixel > 0)
		paletteIndex = spritePalette << 2 | spritePixel;
	else if (backgroundPixel > 0 && spritePixel > 0)
		if (spritePriority == 0)
			paletteIndex = spritePalette << 2 | spritePixel;
		else
			paletteIndex = backgroundPalette << 2 | backgroundPixel;

	Color pixelColor = paletteColors[Read(0x3F00 + paletteIndex) & 0x3F];
	screen->SetPixel(cycle - 1, scanline, pixelColor.red, pixelColor.green, pixelColor.blue);

	if (cycle == 260 && (scanline >= 0 && scanline <= 239 || scanline == 261))
		cartridge->CountPPUScanline();

	cycle++;                   // increment PPU cycle count

	if (cycle == 341)          // 341 cycles in a scanline (0-- 340)
	{
		cycle = 0;
		scanline++;

		if (scanline == 262)   // 262 scanlines in a frame (0 - 261)
		{
			scanline = 0;
			frameComplete = true;        //DisplayBackground();
		}
	}
}

uint8_t PPU::ReadRegister(uint16_t registerAddress)
{
	uint8_t data = 0x00;

	switch (registerAddress)
	{
		case 0x2000:               // control - control register (write-only)
			break;
		case 0x2001:               // mask - mask register (write-only)
			break;
		case 0x2002:               // status - status register (read-only)
			data = statusRegister.reg & 0xE0 | dataBuffer & 0x1F;
			statusRegister.bits.verticalBlank = 0;    // reading PPU status register clears vblank bit 7
			latched = false;                          // reading PPU status register clears address latch used by PPUSCROLL and PPUADDR
			break;
		case 0x2003:               // OAMADDR - object attribute memory address register (write-only)
			break;
		case 0x2004:               // OAMDATA - object attribute memory data register (read/write)
			data = ((uint8_t*)OAM)[oamAddress];
			break;
		case 0x2005:               // PPUSCOLL - scroll register (write-only)
			break; 
		case 0x2006:               // PPUADDR - address register (write-only)
			break;
		case 0x2007:               // PPUDATA - data register (read/write)
			data = dataBuffer;                   // reading from PPU memory is buffered
			dataBuffer = Read(addressV.reg);     // PPU read 
			if (addressV.reg >= 0x3F00)
				data = dataBuffer;               // reading from palette memory is not buffered
			addressV.reg += controlRegister.bits.addressIncrement ? 32 : 1; // increment by 1 (coarseX) or by 32 (coarseY)
			break;
	}

	return data;
}

void PPU::WriteRegister(uint16_t registerAddress, uint8_t data)
{
	switch (registerAddress)
	{
		case 0x2000:               // control - control register (write-only)
			controlRegister.reg = data;
			addressT.bits.baseNametableAddressX = controlRegister.bits.baseNametableAddressX;   // writing control register updates nametable base address bits in temporary VRAM address register
			addressT.bits.baseNametableAddressY = controlRegister.bits.baseNametableAddressY;
			break;
		case 0x2001:               // mask - mask register (write-only)
			maskRegister.reg = data;
			break;
		case 0x2002:               // status - status register (read-only)
			break;
		case 0x2003:               // OAMADDR - object attribute memory address register (write-only)
			oamAddress = data;
			break;
		case 0x2004:               // OAMDATA - object attribute memory data register (read/write)
			((uint8_t*)OAM)[oamAddress] = data;
			break;
		case 0x2005:               // PPUSCOLL - scroll register (write-only)
			if (!latched)          // PPUSCROLL shares the latch register with PPUADDR
			{
				fineX = data & 0x07;
				addressT.bits.coarseX = data >> 3;
				latched = true;
			}
			else  // latched
			{
				addressT.bits.fineY = data & 0x07;
				addressT.bits.coarseY = data >> 3;				
				latched = false;
			}
			break;
		case 0x2006:               // PPUADDR - address register (write-only)
			if (!latched)          // PPUADDR shares the latch register with PPUSCROLL
			{
				addressT.reg = addressT.reg & 0x00FF | (uint16_t)(data & 0x3F) << 8;   // bit 14 is 0 (15 bits register, 14 bits address space)
				latched = true;
			}
			else  // latched
			{
				addressT.reg = addressT.reg & 0xFF00 | data;
				addressV.reg = addressT.reg;
				latched = false;
			}
			break;
		case 0x2007:               // PPUDATA - data register (read/write)
			Write(addressV.reg, data);    // PPU write
			addressV.reg += controlRegister.bits.addressIncrement ? 32 : 1; // increment by 1 (coarseX) or by 32 (coarseY)
			break;
	}
}

uint8_t PPU::Read(uint16_t address)
{
	uint8_t data = 0x00;

	if (address >= 0x0000 && address <= 0x1FFF)          // 8 KiB pattern table
		data = cartridge->PPURead(address);
	else if (address >= 0x2000 && address <= 0x3EFF)     // 4 KiB mirrored VRAM (nametables)
	{
		if (address >= 0x3000)  // mirrored addresses
			address &= 0x2FFF;

		address &= 0x0FFF;      // mask address (VRAM array starts at 0)

		if (cartridge->GetMirroringMode() == MirroringMode::HORIZONTAL)
		{
			if ((address & 1 << 11) == 0)
				data = VRAM[0][address & 0x03FF];
			else  // (address & 1 << 11) != 0
				data = VRAM[1][address & 0x03FF];
		}
		else if (cartridge->GetMirroringMode() == MirroringMode::VERTICAL)
		{
			address &= 0x07FF;  // mask off bit 11

			if ((address & 1 << 10) == 0)
				data = VRAM[0][address & 0x03FF];
			else  // (address & 1 << 10) != 0
				data = VRAM[1][address & 0x03FF];
		}
		else if (cartridge->GetMirroringMode() == MirroringMode::SINGLE_SCREEN_LOWER)
		{
			address &= 0x03FF;

			data = VRAM[0][address];
		}
		else if (cartridge->GetMirroringMode() == MirroringMode::SINGLE_SCREEN_UPPER)
		{
			address &= 0x03FF;

			data = VRAM[1][address];
		}
	}
	else if (address >= 0x3F00 && address <= 0x3FFF)      // 32 bytes palette memory
	{
		address &= 0x3F1F;     // 32 bytes mirrored to 0x3FFF
		address &= 0x001F;     // take only first five bits

		if (address == 0x0010)   // mirrored addresses
			address = 0x0000;
		if (address == 0x0014)
			address = 0x0004;
		if (address == 0x0018)
			address = 0x0008;
		if (address == 0x001C)
			address = 0x000C;

		data = palettes[address];
	}

	return data;
}

void PPU::Write(uint16_t address, uint8_t data)
{
	if (address >= 0x0000 && address <= 0x1FFF)           // 8 KiB pattern table
		cartridge->PPUWrite(address, data);
	else if (address >= 0x2000 && address <= 0x3EFF)      // 4 KiB mirrored VRAM (nametables)
	{
		if (address >= 0x3000)  // mirrored addresses
			address &= 0x2FFF;

		address &= 0x0FFF;      // mask address (VRAM array starts from 0)

		if (cartridge->GetMirroringMode() == MirroringMode::HORIZONTAL)
		{
			if ((address & 1 << 11) == 0)
				VRAM[0][address & 0x03FF] = data;
			else  // (address & 1 << 11) != 0
				VRAM[1][address & 0x03FF] = data;
		}
		else if (cartridge->GetMirroringMode() == MirroringMode::VERTICAL)
		{
			address &= 0x07FF;  // mask off bit 11

			if ((address & 1 << 10) == 0)
				VRAM[0][address & 0x03FF] = data;
			else  // (address & 1 << 10) != 0
				VRAM[1][address & 0x03FF] = data;
		}
		else if (cartridge->GetMirroringMode() == MirroringMode::SINGLE_SCREEN_LOWER)
		{
			address &= 0x03FF;

			VRAM[0][address] = data;
		}
		else if (cartridge->GetMirroringMode() == MirroringMode::SINGLE_SCREEN_UPPER)
		{
			address &= 0x03FF;

			VRAM[1][address] = data;
		}
	}
	else if (address >= 0x3F00 && address <= 0x3FFF)       // 32 bytes palette memory
	{
		address &= 0x3F1F;     // 32 bytes mirrored to 0x3FFF
		address &= 0x001F;     // take only first five bits

		if (address == 0x0010)   // mirrored addresses
			address = 0x0000;
		if (address == 0x0014)
			address = 0x0004;
		if (address == 0x0018)
			address = 0x0008;
		if (address == 0x001C)
			address = 0x000C;

		palettes[address] = data;
	}
}

void PPU::DisplayPatternTable(uint8_t index)
{

}

void PPU::DisplayBackground()
{
	if (!maskRegister.bits.renderBackground)
		return;

	for (int y = 0; y < 30; y++)
		for (int x = 0; x < 32; x++)
		{
			uint8_t tileID = Read((0x2000 | controlRegister.bits.baseNametableAddressY << 11 | controlRegister.bits.baseNametableAddressX << 10) + y * 32 + x);
			
			uint8_t attribute = Read((0x2000 | controlRegister.bits.baseNametableAddressY << 11 | controlRegister.bits.baseNametableAddressX << 10) + 0x03C0 + y / 4 * 8 + x / 4);
			if (x % 4 / 2)
				attribute >>= 2;
			if (y % 4 / 2)
				attribute >>= 4;
			attribute &= 0x03;

			for (int fineY = 0; fineY < 8; fineY++)
			{
				uint8_t backgroundL = Read(controlRegister.bits.backgroundPatternTable * 0x1000 + tileID * 16 + fineY);
				uint8_t backgroundH = Read(controlRegister.bits.backgroundPatternTable * 0x1000 + tileID * 16 + fineY + 8);

				for (int p = 0; p < 8; p++)
				{
					uint8_t paletteindex = (backgroundL & 0x80 >> p) >> 7 - p | (backgroundH & 0x80 >> p) >> 7 - p << 1 | attribute << 2;
					Color color = paletteColors[Read(0x3F00 + paletteindex)];

					screen->SetPixel(x * 8 + p, y * 8 + fineY, color.red, color.green, color.blue);
				}
			}		
		}
}

