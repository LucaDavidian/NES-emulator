#ifndef MAPPER_003_H
#define MAPPER_003_H

#include "mapper.hpp"

/**** iNES mapper 003 ****/
class Mapper003 : public Mapper
{
public:
    Mapper003(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR), mappedCHRBank(0) {}

    void Reset() override {}

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return MirroringMode::HARDWARE; }
private:
    uint8_t mappedCHRBank;
};

#endif  // MAPPER_003_H
