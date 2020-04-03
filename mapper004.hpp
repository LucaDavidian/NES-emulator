#ifndef MAPPER_004_H
#define MAPPER_004_H

#include "mapper.hpp"

/**** iNES mapper 004 (MMC3) ****/
class Mapper004 : public Mapper
{
public:
    Mapper004(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR) { Reset(); }

    void Reset() override;

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return mirroringMode; }

    void CountPPUScanline();
    bool InterruptAsserted() const { return interruptAsserted; }
    void AcknowledgeInterrupt() { interruptAsserted = false; }
private:
    MirroringMode mirroringMode;

    enum class PRGBankMode { FIXED2, FIXED0 } PRGBankMode;
    enum class CHRBankMode { BANKS2KLOW, BANKS2KHIGH } CHRBankMode;

    uint8_t bankSelect;

    uint8_t mappedPRGBank0;   // PRG ROM bank @ 0x8000 - 0x9FFF 
    uint8_t mappedPRGBank1;   // PRG ROM bank @ 0xA000 - 0xBFFF 
    uint8_t mappedPRGBank2;   // PRG ROM bank @ 0xC000 - 0xDFFF
    uint8_t mappedPRGBank3;   // PRG ROM bank @ 0xE000 - 0xFFFF

    uint8_t mappedCHRBank0;
    uint8_t mappedCHRBank1;
    uint8_t mappedCHRBank2;
    uint8_t mappedCHRBank3;
    uint8_t mappedCHRBank4;
    uint8_t mappedCHRBank5;

    bool PRG_RAM_Enabled;
    bool writeProtection;

    bool IRQEnabled;
    uint8_t IRQCounter;       // IRQ counter
    uint8_t IRQReload;        // number of PPU scanlines before firing interrupt
    bool interruptAsserted;
};

#endif  // MAPPER_004_H