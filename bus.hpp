#ifndef BUS_H
#define BUS_H

#include <cstdint>

class CPU;
class PPU;
class APU;
class Cartridge;
class Controller;

class Bus
{
public:
    uint8_t Read(uint16_t address);                
    void Write(uint16_t address, uint8_t data);     

    void ConnectCPU(CPU *cpu) { this->cpu = cpu; }
    void ConnectPPU(PPU *ppu) { this->ppu = ppu; }
    void ConnectAPU(APU *apu) { this->apu = apu; }
    void ConnectController(Controller *controller) { this->controller1 = controller; }
    void InsertCartridge(Cartridge *cartridge);

    void Reset();
    void Clock();
private:
    uint8_t RAM[2048];                    // 2 KiB system RAM  

    uint8_t DMAPage;                      // DMA controller
    uint16_t DMAAddress;
    uint8_t DMAData;
    bool DMAActive = false;
    bool DMADummy = false;

    CPU *cpu = nullptr;                   
    PPU *ppu = nullptr; 
    APU *apu = nullptr;
    Cartridge *cartridge = nullptr;
    Controller *controller1 = nullptr;
    Controller *controller2 = nullptr;

    uint64_t systemClockCount = 0;
};

#endif  // BUS_H