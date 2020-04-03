#include "mapper007.hpp"

/**** iNES mapper 007 ****/

void Mapper007::Reset()
{
	mappedPRGBank = numBanksPRG - 1;
}

uint32_t Mapper007::MapReadPRG(uint16_t address, bool &fromRAM)
{
	if (address >= 0x8000 && address <= 0xFFFF)
		return mappedPRGBank * 0x8000 + (address & 0x7FFF);
}

uint32_t Mapper007::MapWritePRG(uint16_t address, uint8_t data, bool &toRAM)
{
	if (address >= 0x8000 && address <= 0xFFFF)
	{
		mappedPRGBank = data & 0x07;
		mirroringMode = data & 0x10 ? MirroringMode::SINGLE_SCREEN_UPPER : MirroringMode::SINGLE_SCREEN_LOWER;
	}

	return 0x00000000;
}

uint32_t Mapper007::MapReadCHR(uint16_t address)
{
	return address;
}

uint32_t  Mapper007::MapWriteCHR(uint16_t address)
{
	return address;
}