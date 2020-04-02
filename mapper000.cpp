#include "mapper000.hpp"

/**** mapper 000 ****/
// PRG 16/32 KiB
// CHR 8 KiB
uint32_t Mapper000::MapReadPRG(uint16_t address, bool &fromRAM)
{
    uint32_t mappedAddress = 0x00000000;

    if (numBanksPRG == 1)                                           // mapped PRG address == first 14 bits if 16 KiB (0x8000 - 0xBFFF mirrored 0xC000 - 0xFFFF)
        mappedAddress = address &= 0x3FFF;
    else if (numBanksPRG == 2)                                      // mapped PRG address == first 15 bits if 32 KiB (0x8000 - 0xFFFF)
        mappedAddress = address &= 0x7FFF;

    return mappedAddress;
}

uint32_t Mapper000::MapWritePRG(uint16_t address, uint8_t data, bool &toRAM)
{
    uint32_t mappedAddress = 0x00000000;

    if (numBanksPRG == 1)                                           // mapped PRG address == first 14 bits if 16 KiB (0x8000 - 0xBFFF mirrored 0xC000 - 0xFFFF)
        mappedAddress = address &= 0x3FFF;
    else if (numBanksPRG == 2)                                      // mapped PRG address == first 15 bits if 32 KiB (0x8000 - 0xFFFF)
        mappedAddress = address &= 0x7FFF;

    return mappedAddress;
}

uint32_t Mapper000::MapReadCHR(uint16_t address)
{
    return address;
}

uint32_t Mapper000::MapWriteCHR(uint16_t address)
{
    return address;
}