#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>

class Mapper
{
public:
    Mapper(uint8_t numBanksPRG, uint8_t numBanksCHR) : numBanksPRG(numBanksPRG), numBanksCHR(numBanksCHR) {}
    virtual ~Mapper() = default;

    virtual uint16_t MapCPURead(uint16_t address) = 0;
    virtual uint16_t  MapCPUWrite(uint16_t address) = 0;

    virtual uint16_t MapPPURead(uint16_t address) = 0;
    virtual uint16_t  MapPPUWrite(uint16_t address) = 0;
protected:
    uint8_t numBanksPRG;
    uint8_t numBanksCHR;
};

class Mapper0 : public Mapper
{
public:
    Mapper0(uint8_t numBanksPRG, uint8_t numBanksCHR) : Mapper(numBanksPRG, numBanksCHR) {}

    uint16_t MapCPURead(uint16_t address) override;
    uint16_t MapCPUWrite(uint16_t address) override;

    uint16_t MapPPURead(uint16_t address) override;
    uint16_t MapPPUWrite(uint16_t address) override;
};

#endif  // MAPPER_H