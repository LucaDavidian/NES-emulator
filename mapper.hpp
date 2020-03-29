#ifndef MAPPER_H
#define MAPPER_H

#include "cartridge.hpp"
#include <cstdint>

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

/**** iNES mapper 000 ****/
class Mapper0 : public Mapper
{
public:
    Mapper0(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR) {}

    void Reset() override {}

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return MirroringMode::HARDWARE; }
};

/**** iNES mapper 001 (MMC1) ****/
class Mapper1 : public Mapper
{
public:
    Mapper1(uint8_t numBanksPRG, uint8_t numBanksCHR);

    void Reset() override;

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return mirroringMode; }
private:
    uint8_t shiftRegister;

    MirroringMode mirroringMode;
    enum class PRGBankMode { SWITCH_32KB, FIX_LOW_BANK, FIX_HIGH_BANK } PRGBankMode;
    enum class CHRBankMode { SWITCH_8KB, SWITCH_4KB } CHRBankMode;

    bool PRGRAMEnable;

    uint8_t mappedPRGBank32;
    uint8_t mappedPRGBank16Low;
    uint8_t mappedPRGBank16High;
    uint8_t mappedCHRBank8;
    uint8_t mappedCHRBank4Low;
    uint8_t mappedCHRBank4High;
};

/**** iNES mapper 002 ****/
class Mapper2 : public Mapper
{
public:
    Mapper2(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR), mappedPRGBank(0) {}

    void Reset() override {}

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return MirroringMode::HARDWARE; }
private:
    uint8_t mappedPRGBank;
};

/**** iNES mapper 003 ****/
class Mapper3 : public Mapper
{
public:
    Mapper3(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR), mappedCHRBank(0) {}

    void Reset() override {}

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return MirroringMode::HARDWARE; }
private:
    uint8_t mappedCHRBank;
};

/**** iNES mapper 004 (MMC3) ****/
class Mapper3 : public Mapper
{
public:
    Mapper3(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR), mappedCHRBank(0) {}

    void Reset() override {}

    uint32_t MapReadPRG(uint16_t address, bool &fromRAM) override;
    uint32_t MapWritePRG(uint16_t address, uint8_t data, bool &toRAM) override;

    uint32_t MapReadCHR(uint16_t address) override;
    uint32_t MapWriteCHR(uint16_t address) override;

    MirroringMode GetMirroringMode() const override { return MirroringMode::HARDWARE; }
private:
    uint8_t mappedCHRBank;
};


#endif  // MAPPER_H