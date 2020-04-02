#include "mapper003.hpp"

/**** iNES mapper 003 ****/
// PRG 16/32 KiB 
// CHR 32 KiB 
uint32_t Mapper003::MapReadPRG(uint16_t address, bool &fromRAM)
{
    uint32_t mappedAddress = 0x00000000;

    if (numBanksPRG == 1)                                           // mapped PRG address == first 14 bits if 16 KiB (0x8000 - 0xBFFF mirrored 0xC000 - 0xFFFF)
        mappedAddress = address &= 0x3FFF;
    else if (numBanksPRG == 2)                                      // mapped PRG address == first 15 bits if 32 KiB (0x8000 - 0xFFFF)
        mappedAddress = address &= 0x7FFF;

    return mappedAddress;
}

uint32_t Mapper003::MapWritePRG(uint16_t address, uint8_t data, bool &toRAM)
{
    if (address >= 0x8000 && address <= 0xFFFF)                     // writing to 0x8000 - 0xFFFF selects CHR bank
        mappedCHRBank = data;

    return 0x00000000;
}

uint32_t Mapper003::MapReadCHR(uint16_t address)
{
    return mappedCHRBank * 0x2000 + address;                        // mapped CHR address == index of CHR bank * 8 KiB + offset
}

uint32_t Mapper003::MapWriteCHR(uint16_t address)
{
    return mappedCHRBank * 0x2000 + address;                        // mapped CHR address == index of CHR bank * 8 KiB + offset
}