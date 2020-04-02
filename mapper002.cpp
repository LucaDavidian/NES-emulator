#include "mapper002.hpp"

/**** iNES mapper 002 ****/
// PRG 256/4096 KiB 
// CHR 8 KiB
uint32_t Mapper002::MapReadPRG(uint16_t address, bool &fromRAM)
{
    uint32_t mappedAddress = 0x00000000;

    if (address >= 0x8000 && address <= 0xBFFF)                             // low 16 KiB of address space mapped to selected PRG ROM bank
        mappedAddress = mappedPRGBank * 0x4000 + (address & 0x3FFF);        // mapped address == mapped bank number * 16 KiB + offset

    if (address >= 0xC000 && address <= 0xFFFF)                             // last 16 KiB of address space always mapped to last PRG ROM bank
        mappedAddress = (numBanksPRG - 1) * 0x4000 + (address & 0x3FFF);    // mapped address == index of last bank * 16 KiB + offset

    return mappedAddress;
}

uint32_t Mapper002::MapWritePRG(uint16_t address, uint8_t data, bool &toRAM)
{
    if (address >= 0x8000 && address <= 0xFFFF)                    // writing to 0x8000 - 0xFFFF selects PRG bank 
        mappedPRGBank = data & 0x0F;

    return 0x00000000;
}

uint32_t Mapper002::MapReadCHR(uint16_t address)
{
    return address;
}

uint32_t Mapper002::MapWriteCHR(uint16_t address)
{
    return address;
}