#ifndef MAPPER_002_H
#define MAPPER_002_H

#include "mapper.hpp"

/**** iNES mapper 002 ****/
class Mapper002 : public Mapper
{
public:
    Mapper002(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR), mappedPRGBank(0) {}

    void Reset() override {}

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return MirroringMode::HARDWARE; }
private:
    uint8_t mappedPRGBank;
};

#endif  // MAPPER_002_H