#ifndef MAPPER_001_H
#define MAPPER_001_H

#include "mapper.hpp"

/**** iNES mapper 001 (MMC1) ****/
class Mapper001 : public Mapper
{
public:
    Mapper001(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR) { Reset(); }

    void Reset() override;

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return mirroringMode; }
private:
    uint8_t shiftRegister;

    MirroringMode mirroringMode;
    enum class PRGBankMode { SWITCH_32K, FIX_LOW_BANK, FIX_HIGH_BANK } PRGBankMode;
    enum class CHRBankMode { SWITCH_8K, SWITCH_4K } CHRBankMode;

    bool PRG_RAM_Enable;

    uint8_t mappedPRGBank32;
    uint8_t mappedPRGBank16;

    uint8_t mappedCHRBank8;
    uint8_t mappedCHRBank4Low;
    uint8_t mappedCHRBank4High;
};

#endif  // MAPPER_001_H

