#include "mapper004.hpp"

/**** iNES mapper 004 (MMC3) ****/

void Mapper004::Reset()
{
    mirroringMode = MirroringMode::HORIZONTAL;

    PRGBankMode = PRGBankMode::FIXED2;
    CHRBankMode = CHRBankMode::BANKS2KLOW;

    PRG_RAM_Enabled = false;
    writeProtection = false;

    IRQEnabled = false;
    IRQReload = 0x00;
    IRQCounter = 0x00;
    interruptAsserted = false;

    mappedPRGBank0 = 0;
    mappedPRGBank1 = 1;
    mappedPRGBank2 = numBanksPRG - 2;
    mappedPRGBank3 = numBanksPRG - 1;

    mappedCHRBank0 = 0;
    mappedCHRBank1 = 0;
    mappedCHRBank2 = 0;
    mappedCHRBank3 = 0;
    mappedCHRBank4 = 0;
    mappedCHRBank5 = 0;

    bankSelect = 0;
}

uint32_t Mapper004::MapReadPRG(uint16_t address, bool &fromRAM)
{
    uint32_t mappedAddress = 0x00000000;

    if (address >= 0x6000 && address <= 0x7FFF)
    {
        mappedAddress = address & 0x1FFF;
        fromRAM = true;
    }
    else if (address >= 0x8000 && address <= 0x9FFF)
        mappedAddress = mappedPRGBank0 * 0x2000 + (address & 0x1FFF);
    else if (address >= 0xA000 && address <= 0xBFFF)
        mappedAddress = mappedPRGBank1 * 0x2000 + (address & 0x1FFF);
    else if (address >= 0xC000 && address <= 0xDFFF)
        mappedAddress = mappedPRGBank2 * 0x2000 + (address & 0x1FFF);
    else if (address >= 0xE000 && address <= 0xFFFF)
        mappedAddress = mappedPRGBank3 * 0x2000 + (address & 0x1FFF);

    return mappedAddress;
}

uint32_t Mapper004::MapWritePRG(uint16_t address, uint8_t data, bool &toRAM)
{
    uint32_t mappedAddress = 0x00000000;

    if (address >= 0x6000 && address <= 0x7FFF)
    {
        mappedAddress = address & 0x1FFF;
        toRAM = true;
    }
    else if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (!(address & 0x0001))                  // even address: bank select
        {
            bankSelect = data & 0x07;             // select bank to write 

            if (data & 0x40)                      // select PRG bank mode
            {
                PRGBankMode = PRGBankMode::FIXED0;
                mappedPRGBank0 = numBanksPRG - 2;
            }
            else
            {
                PRGBankMode = PRGBankMode::FIXED2;
                mappedPRGBank2 = numBanksPRG - 2;
            }

            if (data & 0x80)                      // select CHR bank mode
                CHRBankMode = CHRBankMode::BANKS2KHIGH;
            else
                CHRBankMode = CHRBankMode::BANKS2KLOW;
        }
        else                                      // odd address: bank data
            switch (bankSelect)
            {
            case 0:
                mappedCHRBank0 = data & 0xFE;
                break;
            case 1:
                mappedCHRBank1 = data & 0xFE;
                break;
            case 2:
                mappedCHRBank2 = data;
                break;
            case 3:
                mappedCHRBank3 = data;
                break;
            case 4:
                mappedCHRBank4 = data;
                break;
            case 5:
                mappedCHRBank5 = data;
                break;
            case 6:
                if (PRGBankMode == PRGBankMode::FIXED0)
                    mappedPRGBank2 = data & 0x3F;
                else if (PRGBankMode == PRGBankMode::FIXED2)
                    mappedPRGBank0 = data & 0x3F;
                break;
            case 7:
                mappedPRGBank1 = data & 0x3F;
                break;
            }
    }
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (!(address & 0x0001))  // even address: mirroring mode
            mirroringMode = data & 0x01 ? MirroringMode::HORIZONTAL : MirroringMode::VERTICAL;
        else                      // odd address: PRG RAM protect
        {
            writeProtection = data & 0x40;
            PRG_RAM_Enabled = data & 0x80;
        }
    }
    else if (address >= 0xC000 && address <= 0xDFFF)
    {
        if (!(address & 0x0001))  // even address: set IRQ reload value
            IRQReload = data;
        else                      // odd address: reload IRQ counter
            IRQCounter = 0x00;    // reload IRQ counter at cycle 260 of scanline
    }
    else if (address >= 0xE000 && address <= 0xFFFF)
    {
        if (!(address & 0x0001))  // even address: disable IRQ
        {
            IRQEnabled = false;
            interruptAsserted = false;
        }
        else                      // odd address: enable IRQ
            IRQEnabled = true;
    }

    return mappedAddress;
}

uint32_t Mapper004::MapReadCHR(uint16_t address)
{
    uint32_t mappedAddress = 0x00000000;

    if (CHRBankMode == CHRBankMode::BANKS2KLOW)
    {
        if (address >= 0x0000 && address <= 0x07FF)            // two 2 KiB banks @ 0x0000 - 0x07FF 
            mappedAddress = mappedCHRBank0 * 0x0400 + (address & 0x07FF);
        else if (address >= 0x0800 && address <= 0x0FFF)       // and 0x0800 - 0x0FFF
            mappedAddress = mappedCHRBank1 * 0x0400 + (address & 0x07FF);
        else if (address >= 0x1000 && address <= 0x13FF)       // four 1 KiB banks @ 0x1000 - 0x13FF
            mappedAddress = mappedCHRBank2 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x1400 && address <= 0x17FF)       // 0x1400 - 0x17FF
            mappedAddress = mappedCHRBank3 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x1800 && address <= 0x1BFF)       // 0x1800 - 0x1BFF
            mappedAddress = mappedCHRBank4 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x1C00 && address <= 0x1FFF)       // 0x1C00 - 0x1FFF
            mappedAddress = mappedCHRBank5 * 0x0400 + (address & 0x03FF);
    }
    else if (CHRBankMode == CHRBankMode::BANKS2KHIGH)
    {
        if (address >= 0x0000 && address <= 0x03FF)            // four 1 KiB banks @ 0x0000 - 0x03FF
            mappedAddress = mappedCHRBank2 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x0400 && address <= 0x07FF)       // 0x0400 - 0x07FF
            mappedAddress = mappedCHRBank3 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x0800 && address <= 0x0BFF)       // 0x0800 - 0x0BFF
            mappedAddress = mappedCHRBank4 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x0C00 && address <= 0x0FFF)       // 0x0C00 - 0x0FFF
            mappedAddress = mappedCHRBank5 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x1000 && address <= 0x17FF)       // two 2 KiB banks @ 0x1000 - 0x17FF 
            mappedAddress = mappedCHRBank0 * 0x0400 + (address & 0x07FF);
        else if (address >= 0x1800 && address <= 0x1FFF)       // and 0x1800 - 0x1FFF
            mappedAddress = mappedCHRBank1 * 0x0400 + (address & 0x07FF);
    }

    return mappedAddress;
}

uint32_t Mapper004::MapWriteCHR(uint16_t address)
{
    uint32_t mappedAddress = 0x00000000;

    if (CHRBankMode == CHRBankMode::BANKS2KLOW)
    {
        if (address >= 0x0000 && address <= 0x07FF)            // two 2 KiB banks @ 0x0000 - 0x07FF 
            mappedAddress = mappedCHRBank0 * 0x0400 + (address & 0x07FF);
        else if (address >= 0x0800 && address <= 0x0FFF)       // and 0x0800 - 0x0FFF
            mappedAddress = mappedCHRBank1 * 0x0400 + (address & 0x07FF);
        else if (address >= 0x1000 && address <= 0x13FF)       // four 1 KiB banks @ 0x1000 - 0x13FF
            mappedAddress = mappedCHRBank2 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x1400 && address <= 0x17FF)       // 0x1400 - 0x17FF
            mappedAddress = mappedCHRBank3 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x1800 && address <= 0x1BFF)       // 0x1800 - 0x1BFF
            mappedAddress = mappedCHRBank4 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x1C00 && address <= 0x1FFF)       // 0x1C00 - 0x1FFF
            mappedAddress = mappedCHRBank5 * 0x0400 + (address & 0x03FF);
    }
    else if (CHRBankMode == CHRBankMode::BANKS2KHIGH)
    {
        if (address >= 0x0000 && address <= 0x03FF)            // four 1 KiB banks @ 0x0000 - 0x03FF
            mappedAddress = mappedCHRBank2 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x0400 && address <= 0x07FF)       // 0x0400 - 0x07FF
            mappedAddress = mappedCHRBank3 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x0800 && address <= 0x0BFF)       // 0x0800 - 0x0BFF
            mappedAddress = mappedCHRBank4 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x0C00 && address <= 0x0FFF)       // 0x0C00 - 0x0FFF
            mappedAddress = mappedCHRBank5 * 0x0400 + (address & 0x03FF);
        else if (address >= 0x1000 && address <= 0x17FF)       // two 2 KiB banks @ 0x1000 - 0x17FF 
            mappedAddress = mappedCHRBank0 * 0x0400 + (address & 0x07FF);
        else if (address >= 0x1800 && address <= 0x1FFF)       // and 0x1800 - 0x1FFF
            mappedAddress = mappedCHRBank1 * 0x0400 + (address & 0x07FF);
    }

    return mappedAddress;
}

void Mapper004::CountPPUScanline()
{
    if (IRQCounter == 0x00)
    {
        IRQCounter = IRQReload;

        if (IRQEnabled)
            interruptAsserted = true;
    }
    else 
        IRQCounter--;
}