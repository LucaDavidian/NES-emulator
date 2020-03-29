#include "cartridge.hpp"
#include "mapper.hpp"
#include "mapper000.hpp"
#include "mapper001.hpp"
#include "mapper002.hpp"
#include "mapper003.hpp"
#include "mapper004.hpp"
#include <fstream>
#include <exception>

class BadFileException : public std::exception {};

void Cartridge::LoadRom(const std::string &ROMFilePath)
{
    std::ifstream ROMFile;

    ROMFile.open(ROMFilePath, std::ios::binary);

    if (!ROMFile)
        throw BadFileException();

    struct Header
    {
        uint8_t NES[3];
        uint8_t eof;
        uint8_t nBanksPRG;
        uint8_t nBanksCHR;
        uint8_t flags6;
        uint8_t flags7;
        uint8_t flags8;
        uint8_t flags9;
        uint8_t flags10;
        uint8_t padding[5];
    } header;

    ROMFile.read((char*)&header, sizeof header);

    // if trainer is present skip 512 bytes
    if (header.flags6 & 0x04)
        ROMFile.seekg(512, std::ios::cur);

    nBanksPRG = header.nBanksPRG; 
    PRG_ROM.Resize(nBanksPRG * 16384);   // a bank of program memory is 16 KiB
    ROMFile.read((char*)PRG_ROM.Data(), PRG_ROM.Size());
    
    nBanksCHR = header.nBanksCHR;  
    if (nBanksCHR == 0)
        nBanksCHR = 1;
    CHR_ROM.Resize(nBanksCHR * 8192);   // a bank of character memory is 8 KiB
    ROMFile.read((char*)CHR_ROM.Data(), CHR_ROM.Size());

    mapperID = header.flags7 & 0xF0 | header.flags6 >> 4; 

    if (header.flags6 & 0x01)
        hardwareMirroringMode = MirroringMode::VERTICAL;
    else  // (header.flags6 & 0x01) == 0
        hardwareMirroringMode = MirroringMode::HORIZONTAL;
    
    switch (mapperID)
    {
        case 0:
            mapper = new Mapper000(nBanksPRG, nBanksCHR);
            break;
        case 1:
            mapper = new Mapper001(nBanksPRG, nBanksCHR);
            break;
        case 2:
            mapper = new Mapper002(nBanksPRG, nBanksCHR);
            break;
        case 3:
            mapper = new Mapper003(nBanksPRG, nBanksCHR);
            break;
        case 4:                                                     // 2 * number of 16 KiB PRG banks -> number of 8 KiB PRG banks
            mapper = new Mapper004(nBanksPRG * 2, nBanksCHR * 8);   // 8 * number of 8 KiB CHR banks -> number of 1 KiB CHR banks
            break;
        default:
            // error
            break;
    }

    ROMFile.close();
}

MirroringMode Cartridge::GetMirroringMode()
{
    MirroringMode mirroringMode = mapper->GetMirroringMode();

    if (mirroringMode == MirroringMode::HARDWARE)    // set by hardware on the cartridge
        return hardwareMirroringMode;            
    else                                             // dynamically set by mapping circuit
        return mirroringMode;                     
}

uint8_t Cartridge::CPURead(uint16_t address)   // CPU read 0x4020 - 0xFFFF
{
    bool fromRAM = false;

    uint32_t mappedAddress = mapper->MapReadPRG(address, fromRAM);

    if (fromRAM)
        return PRG_RAM[mappedAddress];
    else
        return PRG_ROM[mappedAddress];
}

void Cartridge::CPUWrite(uint16_t address, uint8_t data)   // CPU write 0x4020 - 0xFFFF
{
    bool toRAM = false;

    uint32_t mappedAddress = mapper->MapWritePRG(address, data, toRAM);   

    if (toRAM)
        PRG_RAM[mappedAddress] = data;
}

uint8_t Cartridge::PPURead(uint16_t address)   // PPU read 0x0000 - 0x1FFFF
{
    uint32_t mappedAddress = mapper->MapReadCHR(address);

    return CHR_ROM[mappedAddress];
}

void Cartridge::PPUWrite(uint16_t address, uint8_t data)  // PPU write 0x0000 - 0x1FFFF
{
    uint32_t mappedAddress = mapper->MapWriteCHR(address);

    CHR_ROM[mappedAddress] = data;
}

bool Cartridge::InterruptAsserted()
{
    if (mapperID == 4)
        return static_cast<Mapper004*>(mapper)->InterruptAsserted();
    
    return false;
}

void Cartridge::AcknowledgeInterrupt()
{
    if (mapperID == 4)
        static_cast<Mapper004*>(mapper)->AcknowledgeInterrupt();
}

void Cartridge::CountPPUScanline()
{
    if (mapperID == 4)
        static_cast<Mapper004*>(mapper)->CountPPUScanline();
}