#include "mapper.hpp"

uint16_t Mapper0::MapCPURead(uint16_t address)
{
    address = address & (numBanksPRG > 1 ? 0x7FFF : 0x3FFF);

    return address;
}

uint16_t Mapper0::MapCPUWrite(uint16_t address)
{
    address = address & (numBanksPRG > 1 ? 0x7FFF : 0x3FFF);

    return address;
}

uint16_t Mapper0::MapPPURead(uint16_t address)
{
    return address;
}

uint16_t Mapper0::MapPPUWrite(uint16_t address)
{
    return address;
}