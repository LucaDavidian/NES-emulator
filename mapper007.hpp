#ifndef MAPPER_007
#define MAPPER_007

#include "mapper.hpp"

/**** iNES mapper 007 ****/
class Mapper007 : public Mapper
{
public:
    Mapper007(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR) { Reset(); }

    void Reset() override;

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return mirroringMode; }
private:
    uint8_t mappedPRGBank;

    MirroringMode mirroringMode;
};

#endif  // MAPPER_007
