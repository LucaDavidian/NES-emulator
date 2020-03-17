#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "../../../data structures/vector/vector.hpp"
#include <cstdint>
#include <string>
#include "mapper.hpp"

class Cartridge
{
public:
    void LoadRom(const std::string &ROMFilePath);
    
    uint8_t CPURead(uint16_t address);
    void CPUWrite(uint16_t address, uint8_t data);

    uint8_t PPURead(uint16_t address);
    void PPUWrite(uint16_t address, uint8_t data);

    enum class MirroringMode { HORIZONTAL, VERTICAL, };
    MirroringMode mirroringMode;
private:
    Vector<uint8_t> PRG_ROM;
    Vector<uint8_t> CHR_ROM;

    uint8_t nBanksPRG;
    uint8_t nBanksCHR;
    uint8_t mapperID;

    Mapper *mapper;
};

#endif  // CARTRIDGE_H