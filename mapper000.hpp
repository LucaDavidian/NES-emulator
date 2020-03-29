#ifndef MAPPER_000_H
#define MAPPER_000_H

#include "mapper.hpp"

/**** iNES mapper 000 ****/
class Mapper000 : public Mapper
{
public:
    Mapper000(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR) {}

    void Reset() override {}

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return MirroringMode::HARDWARE; }
};

#endif  // MAPPER_000_H
