#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>
#include "cartridge.hpp"

class Mapper
{
public:
    Mapper(uint8_t numBanksPRG, uint8_t numBanksCHR) : numBanksPRG(numBanksPRG), numBanksCHR(numBanksCHR) {}
    virtual ~Mapper() = default;

    virtual void Reset() = 0;

    virtual uint32_t MapReadPRG(uint16_t address, bool &fromRAM) = 0;
    virtual uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) = 0;

    virtual uint32_t MapReadCHR(uint16_t address) = 0;
    virtual uint32_t  MapWriteCHR(uint16_t address) = 0;

    virtual MirroringMode GetMirroringMode() const = 0;
protected:
    uint8_t numBanksPRG;
    uint8_t numBanksCHR;
};

#endif  // MAPPER_H