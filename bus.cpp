#include "bus.hpp"
#include "CPU.hpp"
#include "PPU.hpp"
#include "APU.hpp"
#include "cartridge.hpp"
#include "controller.h"

void Bus::Reset() 
{
    cpu->Reset(); 
    ppu->Reset();
    apu->Reset();

    systemClockCount = 0; 
}

void Bus::InsertCartridge(Cartridge *cartridge) 
{ 
    this->cartridge = cartridge; 
    
    ppu->ConnectCartridge(cartridge); 
}

uint8_t Bus::Read(uint16_t address)
{
    uint8_t data = 0x00;

    if (address >= 0x0000 && address <= 0x1FFF)          // access RAM
        data = RAM[address & 0x07FF];
    else if (address >= 0x2000 && address <= 0x3FFF)     // access PPU
        data = ppu->ReadRegister(address); 
    else if (address == 0x4016 || address == 0x4017)     // read controller state
    {
        if (address == 0x4016)           // controller 1
            if (controller1 && controller1->GetState())
                data = 0x01;

        if (address == 0x4017)           // controller 2
            if (controller2 && controller2->GetState())
                data = 0x01;
    } 
    else  // address >= 0x4020U && address <= 0xFFFF     // access cartridge
        data = cartridge->CPURead(address);

    return data;
}

void Bus::Write(uint16_t address, uint8_t data)
{
    if (address >= 0x0000U && address <= 0x1FFF)         // access RAM
        RAM[address & 0x07FF] = data;
    else if (address >= 0x2000 && address <= 0x3FFF)     // access PPU
        ppu->WriteRegister(address, data);
    else if (address >= 0x4000 && address <= 0x4013 || address == 0x4015 || address == 0x4017)   // access APU
        apu->WriteRegister(address, data);
    else if (address == 0x4014)                          // access OAM DMA
    {
        DMAPage = 0x01 * data;
        DMAAddress = (uint16_t)DMAPage << 8;
        DMAActive = true;
        DMADummy = true;
    }
    else if (address == 0x4016)                          // latch controller state
    {
        if (controller1 && !(data & 0x01))
            controller1->PollButtons();

        if (controller2 && !(data & 0x01))
            controller2->PollButtons();
    }
    else  // address >= 0x4020U && address <= 0xFFFF     // access cartridge
        cartridge->CPUWrite(address, data);
}

void Bus::Clock()      // TODO: CPU should be done with instruction before triggering NMI (or being suspended by DMA)
{
    ppu->Clock();   

    apu->Clock();      

    if (systemClockCount % 3 == 0)                    // CPU clock = PPU clock / 3, DMA and CPU share same clock
        if (DMAActive)                                // if DMA is active, suspend CPU
        {
            if (DMADummy)                             // dummy DMA cycle
            {
                if (systemClockCount / 3 % 2)         // another dummy cycle if CPU/DMA clock cycle is odd
                    DMADummy = false;
            }
            else
            {
                if (systemClockCount / 3 % 2 == 0)    // even CPU/DMA cycle: read data from system RAM
                    DMAData = Read(DMAAddress);
                else                                  // odd CPU/DMA cycle: write data to OAM
                {
                    Write(0x2003, (uint8_t)(DMAAddress & 0x00FF));
                    Write(0x2004, DMAData);

                    DMAAddress++;

                    if (DMAAddress >> 8 != DMAPage)   // if 256 bytes have been written, disable DMA
                        DMAActive = false;
                }
            }
        }
        else                                           // if DMA is not active clock CPU
            cpu->Clock();

    if (ppu->InterruptAsserted())    // check if PPU has triggered the vertical blank interrupt
    {
        ppu->AcknowledgeInterrupt();
        cpu->NMI();  
    }

    if (cartridge->InterruptAsserted())
    {
        cartridge->AcknowledgeInterrupt();
        cpu->IRQ();
    }

    systemClockCount++;
}