#include "cartridge.hpp"
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

    mapperID = header.flags7 & 0xF0 | header.flags6 >> 4U; 

    if (header.flags6 & 0x01)
        mirroringMode = MirroringMode::VERTICAL;
    else
        mirroringMode = MirroringMode::HORIZONTAL;
    
    switch (mapperID)
    {
        case 0:
            mapper = new Mapper0(nBanksPRG, nBanksCHR);
            break;
        default:
            mapper = new Mapper0(nBanksPRG, nBanksCHR);
    }

    ROMFile.close();
}

uint8_t Cartridge::CPURead(uint16_t address)
{
    uint16_t mappedAddress = mapper->MapCPURead(address);

    return PRG_ROM[mappedAddress];
}

void Cartridge::CPUWrite(uint16_t address, uint8_t data)
{
    uint16_t mappedAddress = mapper->MapCPUWrite(address);

    PRG_ROM[mappedAddress] = data;
}

uint8_t Cartridge::PPURead(uint16_t address)
{
    uint16_t mappedAddress = mapper->MapPPURead(address);

    return CHR_ROM[mappedAddress];
}

void Cartridge::PPUWrite(uint16_t address, uint8_t data)
{
    uint16_t mappedAddress = mapper->MapPPUWrite(address);

    CHR_ROM[mappedAddress] = data;
}