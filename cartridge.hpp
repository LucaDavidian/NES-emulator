#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "../../../data structures/vector/vector.hpp"
#include <cstdint>
#include <string>

class Mapper;

enum class MirroringMode { HARDWARE, HORIZONTAL, VERTICAL, SINGLE_SCREEN_LOWER, SINGLE_SCREEN_UPPER };

class Cartridge
{
public:
    void LoadRom(const std::string &ROMFilePath);
    
    uint8_t CPURead(uint16_t address);
    void CPUWrite(uint16_t address, uint8_t data);

    uint8_t PPURead(uint16_t address);
    void PPUWrite(uint16_t address, uint8_t data);

    MirroringMode GetMirroringMode();

    bool InterruptAsserted();
    void AcknowledgeInterrupt();

    void CountPPUScanline();
private:
    Vector<uint8_t> PRG_ROM;  // PRG ROM
    Vector<uint8_t> CHR_ROM;  // CHR ROM

    Vector<uint8_t> PRG_RAM;  // optional PRG RAM

    uint8_t nBanksPRG;
    uint8_t nBanksCHR;
    uint8_t mapperID;

    Mapper *mapper;

    MirroringMode hardwareMirroringMode;
};

#endif  // CARTRIDGE_H