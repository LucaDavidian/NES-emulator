#include "mapper001.hpp"

/**** mapper 001 (MMC1) ****/
// PRG 256/512 KiB
// CHR 128 KiB
void Mapper001::Reset()
{
    PRG_RAM_Enable = false;

    PRGBankMode = PRGBankMode::FIX_HIGH_BANK;
    CHRBankMode = CHRBankMode::SWITCH_4KB;

    mappedPRGBank16Low = 0;
    mappedPRGBank16High = numBanksPRG - 1;
    mappedCHRBank4Low = 0;
    mappedCHRBank4High = 0;

    shiftRegister = 0x00;
}

uint32_t Mapper001::MapReadPRG(uint16_t address, bool &fromRAM)
{
    uint32_t mappedAddress = 0x00000000;

    if (PRG_RAM_Enable && address >= 0x6000 && address <= 0x7FFF)  // 8 KiB PRG RAM (optional)
    {
        mappedAddress = address & 0x1FFF;
        fromRAM = true;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
        switch (PRGBankMode)
        {
        case PRGBankMode::SWITCH_32KB:
            mappedAddress = mappedPRGBank32 * 0x8000 + (address & 0x7FFF);  // 0x8000 - 0xFFFF
            break;
        case PRGBankMode::FIX_LOW_BANK:
        case PRGBankMode::FIX_HIGH_BANK:
            if (address <= 0xBFFF)    // 0x8000 - 0xBFFF
                mappedAddress = mappedPRGBank16Low * 0x4000 + (address & 0x3FFF);
            else                      //  0xC000 - 0xFFFF
                mappedAddress = mappedPRGBank16High * 0x4000 + (address & 0x3FFF);
            break;
        }

    return mappedAddress;
}

uint32_t Mapper001::MapWritePRG(uint16_t address, uint8_t data, bool &toRAM)
{
    static uint8_t shift;

    uint32_t mappedAddress = 0x00000000;

    if (PRG_RAM_Enable && address >= 0x6000 && address <= 0x7FFFF)  // 8 KiB PRG RAM (optional)
    {
        mappedAddress = address & 0x1FFF;
        toRAM = true;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        if (data & 0x80)
        {
            shiftRegister = 0x00;   // writing a byte with bit 7 set clears shift register
            shift = 0;              // reset shift register
        }
        else
        {
            if (shift < 4)
            {
                shiftRegister = shiftRegister >> 1 | (data & 0x01) << 3;  // shift bit 0 of data into 4-bit shift register (LSB)
                shift++;
            }
            else
            {
                uint8_t internalRegister = shiftRegister | (data & 0x01) << 4;  // 5-bit internal register

                shiftRegister = 0x00;                                           // writing fifth bit of command clears shift register
                shift = 0;                                                      // reset shift register

                if (!(address & 0x4000) && !(address & 0x2000))       // control register: address 0x8000 - 0x9FFF, bit 14 == 0, bit 13 == 0
                {
                    switch (internalRegister & 0x03)
                    {
                    case 0:
                        mirroringMode = MirroringMode::SINGLE_SCREEN_LOWER;
                        break;
                    case 1:
                        mirroringMode = MirroringMode::SINGLE_SCREEN_UPPER;
                        break;
                    case 2:
                        mirroringMode = MirroringMode::VERTICAL;
                        break;
                    case 3:
                        mirroringMode = MirroringMode::HORIZONTAL;
                        break;
                    }

                    switch ((internalRegister & 0x0C) >> 2)
                    {
                    case 0:
                    case 1:
                        PRGBankMode = PRGBankMode::SWITCH_32KB;
                        break;
                    case 2:
                        PRGBankMode = PRGBankMode::FIX_LOW_BANK;
                        break;
                    case 3:
                        PRGBankMode = PRGBankMode::FIX_HIGH_BANK;
                        break;
                    }

                    CHRBankMode = internalRegister & 0x10 ? CHRBankMode::SWITCH_4KB : CHRBankMode::SWITCH_8KB;
                }
                else if (!(address & 0x4000) && (address & 0x2000))   // CHR bank 0: address 0xA000 - 0xBFFF, bit 14 == 0, bit 13 == 1
                {
                    if (CHRBankMode == CHRBankMode::SWITCH_8KB)
                        mappedCHRBank8 = internalRegister >> 1;
                    else if (CHRBankMode == CHRBankMode::SWITCH_4KB)
                        mappedCHRBank4Low = internalRegister;
                }
                else if ((address & 0x4000) && !(address & 0x2000))   // CHR bank 1: address 0xC000 - 0xDFFF, bit 14 == 1, bit 13 == 0
                {
                    if (CHRBankMode == CHRBankMode::SWITCH_4KB)
                        mappedCHRBank4High = internalRegister;
                }
                else if ((address & 0x4000) && (address & 0x2000))    // PRG bank: address 0xE000 - 0xFFFF, bit 14 == 1, bit 13 == 1
                {
                    if (PRGBankMode == PRGBankMode::SWITCH_32KB)
                        mappedPRGBank32 = internalRegister >> 1 & 0x07;
                    else if (PRGBankMode == PRGBankMode::FIX_LOW_BANK)
                    {
                        mappedPRGBank16Low = 0;
                        mappedPRGBank16High = internalRegister & 0x0F;
                    }
                    else if (PRGBankMode == PRGBankMode::FIX_HIGH_BANK)
                    {
                        mappedPRGBank16Low = internalRegister & 0x0F;
                        mappedPRGBank16High = numBanksPRG - 1;
                    }

                    PRG_RAM_Enable = internalRegister & 0x10;
                }
            }
        }
    }

    return mappedAddress;
}

uint32_t Mapper001::MapReadCHR(uint16_t address)
{
    uint32_t mappedAddress = 0x00000000;

    if (CHRBankMode == CHRBankMode::SWITCH_4KB)
        if (address <= 0x0FFF)   // 0x0000 - 0x0FFF
            mappedAddress = mappedCHRBank4Low * 0x1000 + address;
        else                     // 0x1000 - 0x1FFF
            mappedAddress = mappedCHRBank4High * 0x1000 + (address & 0x0FFF);
    else if (CHRBankMode == CHRBankMode::SWITCH_8KB)
        mappedAddress = mappedCHRBank4Low * 0x2000 + address;

    return mappedAddress;
}

uint32_t Mapper001::MapWriteCHR(uint16_t address)
{
    uint32_t mappedAddress = 0x00000000;

    if (CHRBankMode == CHRBankMode::SWITCH_4KB)
        if (address <= 0x0FFF)   // 0x0000 - 0x0FFF
            mappedAddress = mappedCHRBank4Low * 0x1000 + address;
        else                     // 0x1000 - 0x1FFF
            mappedAddress = mappedCHRBank4High * 0x1000 + (address & 0x0FFF);
    else if (CHRBankMode == CHRBankMode::SWITCH_8KB)
        mappedAddress = mappedCHRBank4Low * 0x2000 + address;

    return mappedAddress;
}