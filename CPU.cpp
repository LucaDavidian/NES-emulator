#include "CPU.hpp"
#include "bus.hpp"

#include <iostream>
#include <fstream>
#include "Error.h"

CPU::CPU(Bus &bus) : instructionTable
    {           /* 0x00 */                                    /* 0x01 */                                          /* 0x02 */                                      /* 0x03 */                                      /* 0x04 */                                          /* 0x05 */                                           /* 0x06 */                                           /* 0x07 */                                      /* 0x08 */                                  /* 0x09 */                                           /* 0x0A */                                      /* 0x0B */                                      /* 0x0C */                                           /* 0x0D */                                           /* 0x0E */                                           /* 0x0F */                                                                                                                                                  
    /* 0x00 */  {"BRK", &CPU::Addr_Implied, &CPU::BRK, 7},    {"ORA", &CPU::Addr_IndexedIndirect, &CPU::ORA, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"ORA", &CPU::Addr_ZeroPage, &CPU::ORA, 3},          {"ASL", &CPU::Addr_ZeroPage, &CPU::ASL, 5},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"PHP", &CPU::Addr_Implied, &CPU::PHP, 3},  {"ORA", &CPU::Addr_Immediate, &CPU::ORA, 2},         {"ASL", &CPU::Addr_Accumulator, &CPU::ASL, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"ORA", &CPU::Addr_Absolute, &CPU::ORA, 4},          {"ASL", &CPU::Addr_Absolute, &CPU::ASL, 6},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x10 */  {"BPL", &CPU::Addr_Relative, &CPU::BPL, 2},   {"ORA", &CPU::Addr_IndirectIndexed, &CPU::ORA, 5},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"ORA", &CPU::Addr_ZeroPageIndexedX, &CPU::ORA, 4},  {"ASL", &CPU::Addr_ZeroPageIndexedX, &CPU::ASL, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"CLC", &CPU::Addr_Implied, &CPU::CLC, 2},  {"ORA", &CPU::Addr_AbsoluteIndexedY, &CPU::ORA, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"ORA", &CPU::Addr_AbsoluteIndexedX, &CPU::ORA, 4},  {"ASL", &CPU::Addr_AbsoluteIndexedX, &CPU::ASL, 7},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x20 */  {"JSR", &CPU::Addr_Absolute, &CPU::JSR, 6},   {"AND", &CPU::Addr_IndexedIndirect, &CPU::AND, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"BIT", &CPU::Addr_ZeroPage, &CPU::BIT, 3},         {"AND", &CPU::Addr_ZeroPage, &CPU::AND, 3},          {"ROL", &CPU::Addr_ZeroPage, &CPU::ROL, 5},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"PLP", &CPU::Addr_Implied, &CPU::PLP, 4},  {"AND", &CPU::Addr_Immediate, &CPU::AND, 2},         {"ROL", &CPU::Addr_Accumulator, &CPU::ROL, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"BIT", &CPU::Addr_Absolute, &CPU::BIT, 4},          {"AND", &CPU::Addr_Absolute, &CPU::AND, 4},          {"ROL", &CPU::Addr_Absolute, &CPU::ROL, 6},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x30 */  {"BMI", &CPU::Addr_Relative, &CPU::BMI, 2},   {"AND", &CPU::Addr_IndirectIndexed, &CPU::AND, 5},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"AND", &CPU::Addr_ZeroPageIndexedX, &CPU::AND, 4},  {"ROL", &CPU::Addr_ZeroPageIndexedX, &CPU::ROL, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"SEC", &CPU::Addr_Implied, &CPU::SEC, 2},  {"AND", &CPU::Addr_AbsoluteIndexedY, &CPU::AND, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"AND", &CPU::Addr_AbsoluteIndexedX, &CPU::AND, 4},  {"ROL", &CPU::Addr_AbsoluteIndexedX, &CPU::ROL, 7},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x40 */  {"RTI", &CPU::Addr_Implied, &CPU::RTI, 6},    {"EOR", &CPU::Addr_IndexedIndirect, &CPU::EOR, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"EOR", &CPU::Addr_ZeroPage, &CPU::EOR, 3},          {"LSR", &CPU::Addr_ZeroPage, &CPU::LSR, 5},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"PHA", &CPU::Addr_Implied, &CPU::PHA, 3},  {"EOR", &CPU::Addr_Immediate, &CPU::EOR, 2},         {"LSR", &CPU::Addr_Accumulator, &CPU::LSR, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"JMP", &CPU::Addr_Absolute, &CPU::JMP, 3},          {"EOR", &CPU::Addr_Absolute, &CPU::EOR, 4},          {"LSR", &CPU::Addr_Absolute, &CPU::LSR, 6},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x50 */  {"BVC", &CPU::Addr_Relative, &CPU::BVC, 2},   {"EOR", &CPU::Addr_IndirectIndexed, &CPU::EOR, 5},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"EOR", &CPU::Addr_ZeroPageIndexedX, &CPU::EOR, 4},  {"LSR", &CPU::Addr_ZeroPageIndexedX, &CPU::LSR, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"CLI", &CPU::Addr_Implied, &CPU::CLI, 2},  {"EOR", &CPU::Addr_AbsoluteIndexedY, &CPU::EOR, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"EOR", &CPU::Addr_AbsoluteIndexedX, &CPU::EOR, 4},  {"LSR", &CPU::Addr_AbsoluteIndexedX, &CPU::LSR, 7},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x60 */  {"RTS", &CPU::Addr_Implied, &CPU::RTS, 6},    {"ADC", &CPU::Addr_IndexedIndirect, &CPU::ADC, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"ADC", &CPU::Addr_ZeroPage, &CPU::ADC, 3},          {"ROR", &CPU::Addr_ZeroPage, &CPU::ROR, 5},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"PLA", &CPU::Addr_Implied, &CPU::PLA, 4},  {"ADC", &CPU::Addr_Immediate, &CPU::ADC, 2},         {"ROR", &CPU::Addr_Accumulator, &CPU::ROR, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"JMP", &CPU::Addr_AbsoluteIndirect, &CPU::JMP, 5},  {"ADC", &CPU::Addr_Absolute, &CPU::ADC, 4},          {"ROR", &CPU::Addr_Absolute, &CPU::ROR, 6},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x70 */  {"BVS", &CPU::Addr_Relative, &CPU::BVS, 2},   {"ADC", &CPU::Addr_IndirectIndexed, &CPU::ADC, 5},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"ADC", &CPU::Addr_ZeroPageIndexedX, &CPU::ADC, 4},  {"ROR", &CPU::Addr_ZeroPageIndexedX, &CPU::ROR, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"SEI", &CPU::Addr_Implied, &CPU::SEI, 2},  {"ADC", &CPU::Addr_AbsoluteIndexedY, &CPU::ADC, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"ADC", &CPU::Addr_AbsoluteIndexedX, &CPU::ADC, 4},  {"ROR", &CPU::Addr_AbsoluteIndexedX, &CPU::ROR, 7},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x80 */  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},{"STA", &CPU::Addr_IndexedIndirect, &CPU::STA, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"STY", &CPU::Addr_ZeroPage, &CPU::STY, 3},         {"STA", &CPU::Addr_ZeroPage, &CPU::STA, 3},          {"STX", &CPU::Addr_ZeroPage, &CPU::STX, 3},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"DEY", &CPU::Addr_Implied, &CPU::DEY, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"TXA", &CPU::Addr_Implied, &CPU::TXA, 2},      {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"STY", &CPU::Addr_Absolute, &CPU::STY, 4},          {"STA", &CPU::Addr_Absolute, &CPU::STA, 4},          {"STX", &CPU::Addr_Absolute, &CPU::STX, 4},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0x90 */  {"BCC", &CPU::Addr_Relative, &CPU::BCC, 2},   {"STA", &CPU::Addr_IndirectIndexed, &CPU::STA, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"STY", &CPU::Addr_ZeroPageIndexedX, &CPU::STY, 4}, {"STA", &CPU::Addr_ZeroPageIndexedX, &CPU::STA, 4},  {"STX", &CPU::Addr_ZeroPageIndexedY, &CPU::STX, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"TYA", &CPU::Addr_Implied, &CPU::TYA, 2},  {"STA", &CPU::Addr_AbsoluteIndexedY, &CPU::STA, 5},  {"TXS", &CPU::Addr_Implied, &CPU::TXS, 2},      {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"STA", &CPU::Addr_AbsoluteIndexedX, &CPU::STA, 5},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0xA0 */  {"LDY", &CPU::Addr_Immediate, &CPU::LDY, 2},  {"LDA", &CPU::Addr_IndexedIndirect, &CPU::LDA, 6},  {"LDX", &CPU::Addr_Immediate, &CPU::LDX, 2},    {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"LDY", &CPU::Addr_ZeroPage, &CPU::LDY, 3},         {"LDA", &CPU::Addr_ZeroPage, &CPU::LDA, 3},          {"LDX", &CPU::Addr_ZeroPage, &CPU::LDX, 3},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"TAY", &CPU::Addr_Implied, &CPU::TAY, 2},  {"LDA", &CPU::Addr_Immediate, &CPU::LDA, 2},         {"TAX", &CPU::Addr_Implied, &CPU::TAX, 2},      {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"LDY", &CPU::Addr_Absolute, &CPU::LDY, 4},          {"LDA", &CPU::Addr_Absolute, &CPU::LDA, 4},          {"LDX", &CPU::Addr_Absolute, &CPU::LDX, 4},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0xB0 */  {"BCS", &CPU::Addr_Relative, &CPU::BCS, 2},   {"LDA", &CPU::Addr_IndirectIndexed, &CPU::LDA, 5},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"LDY", &CPU::Addr_ZeroPageIndexedX, &CPU::LDY, 4}, {"LDA", &CPU::Addr_ZeroPageIndexedX, &CPU::LDA, 4},  {"LDX", &CPU::Addr_ZeroPageIndexedY, &CPU::LDX, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"CLV", &CPU::Addr_Implied, &CPU::CLV, 2},  {"LDA", &CPU::Addr_AbsoluteIndexedY, &CPU::LDA, 4},  {"TSX", &CPU::Addr_Implied, &CPU::TSX, 2},      {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"LDY", &CPU::Addr_AbsoluteIndexedX, &CPU::LDY, 4},  {"LDA", &CPU::Addr_AbsoluteIndexedX, &CPU::LDA, 4},  {"LDX", &CPU::Addr_AbsoluteIndexedY, &CPU::LDX, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0xC0 */  {"CPY", &CPU::Addr_Immediate, &CPU::CPY, 2},  {"CMP", &CPU::Addr_IndexedIndirect, &CPU::CMP, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"CPY", &CPU::Addr_ZeroPage, &CPU::CPY, 3},         {"CMP", &CPU::Addr_ZeroPage, &CPU::CMP, 3},          {"DEC", &CPU::Addr_ZeroPage, &CPU::DEC, 5},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INY", &CPU::Addr_Implied, &CPU::INY, 2},  {"CMP", &CPU::Addr_Immediate, &CPU::CMP, 2},         {"DEX", &CPU::Addr_Implied, &CPU::DEX, 2},      {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"CPY", &CPU::Addr_Absolute, &CPU::CPY, 4},          {"CMP", &CPU::Addr_Absolute, &CPU::CMP, 4},          {"DEC", &CPU::Addr_Absolute, &CPU::DEC, 6},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0xD0 */  {"BNE", &CPU::Addr_Relative, &CPU::BNE, 2},   {"CMP", &CPU::Addr_IndirectIndexed, &CPU::CMP, 5},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"CMP", &CPU::Addr_ZeroPageIndexedX, &CPU::CMP, 4},  {"DEC", &CPU::Addr_ZeroPageIndexedX, &CPU::DEC, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"CLD", &CPU::Addr_Implied, &CPU::CLD, 2},  {"CMP", &CPU::Addr_AbsoluteIndexedY, &CPU::CMP, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"CMP", &CPU::Addr_AbsoluteIndexedX, &CPU::CMP, 4},  {"DEC", &CPU::Addr_AbsoluteIndexedX, &CPU::DEC, 7},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0xE0 */  {"CPX", &CPU::Addr_Immediate, &CPU::CPX, 2},  {"SBC", &CPU::Addr_IndexedIndirect, &CPU::SBC, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"CPX", &CPU::Addr_ZeroPage, &CPU::CPX, 3},         {"SBC", &CPU::Addr_ZeroPage, &CPU::SBC, 3},          {"INC", &CPU::Addr_ZeroPage, &CPU::INC, 5},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INX", &CPU::Addr_Implied, &CPU::INX, 2},  {"SBC", &CPU::Addr_Immediate, &CPU::SBC, 2},         {"NOP", &CPU::Addr_Implied, &CPU::NOP, 2},      {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"CPX", &CPU::Addr_Absolute, &CPU::CPX, 4},          {"SBC", &CPU::Addr_Absolute, &CPU::SBC, 4},          {"INC", &CPU::Addr_Absolute, &CPU::INC, 6},          {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    /* 0xF0 */  {"BEQ", &CPU::Addr_Relative, &CPU::BEQ, 2},   {"SBC", &CPU::Addr_IndirectIndexed, &CPU::SBC, 5},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},      {"SBC", &CPU::Addr_ZeroPageIndexedX, &CPU::SBC, 4},  {"INC", &CPU::Addr_ZeroPageIndexedX, &CPU::INC, 6},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"SED", &CPU::Addr_Implied, &CPU::SED, 2},  {"SBC", &CPU::Addr_AbsoluteIndexedY, &CPU::SBC, 4},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},       {"SBC", &CPU::Addr_AbsoluteIndexedX, &CPU::SBC, 4},  {"INC", &CPU::Addr_AbsoluteIndexedX, &CPU::INC, 7},  {"INVALID", &CPU::Addr_Implied, &CPU::NOP, 2},
    }, bus(bus)
{
}

uint8_t CPU::Read(uint16_t address)
{
    return bus.Read(address);
}

void CPU::Write(uint16_t address, uint8_t data)
{
    bus.Write(address, data);
}

void CPU::SetFlag(Flag flag, bool condition)
{   
    if (condition)
        P.reg |= 1 << static_cast<unsigned>(flag);
    else
        P.reg &= ~(1 << static_cast<unsigned>(flag));
}

bool CPU::GetFlag(Flag flag)
{
    return P.reg & 1 << static_cast<unsigned>(flag);
}

void CPU::Reset()
{
    A = 0x00;
    X = 0x00;
    Y = 0x00;

    S = 0xFD;
    P.reg = 0x24;

    address = 0x0000;
    opcode = 0x00;
    currentInstructionCycles = 0;
    totalCycles = 7;

    // get reset vector
    uint8_t resetVectorL = Read(0xFFFC);
    uint8_t resetVectorH = Read(0xFFFD);

    uint16_t resetVector = (uint16_t)resetVectorH << 8U | resetVectorL;

    // jump to reset vector
    PC = resetVector;
}

void CPU::NMIHandler()
{
    Write(stackBase + (uint16_t)S--, PC >> 8 & 0x00FF);     // push PCH on stack
    Write(stackBase + (uint16_t)S--, PC & 0x00FF);          // push PCL on stack
    
    // clear break flag
    SetFlag(Flag::B, false);
    
    Write(stackBase + (uint16_t)S--, P.reg);                // push processor status register (with B flag clear) on stack

    // set interrupt disable flag
    SetFlag(Flag::I, true);

    // get NMI vector
    uint8_t NMIVectorL = Read(0xFFFA);
    uint8_t NMIVectorH = Read(0xFFFB);

    uint16_t NMIVector = (uint16_t)NMIVectorH << 8 | NMIVectorL;

    // jump to NMI vector
    PC = NMIVector;
}

void CPU::IRQHandler()
{
    // if interrupts are disabled ignore interrupt
    if (GetFlag(Flag::I))
        return;

    Write(stackBase + (uint16_t)S--, PC >> 8 & 0x00FF);     // push PCH on stack
    Write(stackBase + (uint16_t)S--, PC & 0x00FF);          // push PCL on stack

    // clear break flag
    SetFlag(Flag::B, false);

    Write(stackBase + (uint16_t)S--, P.reg);                // push processor status register (with B flag clear) on stack

    // set interrupt disable flag
    SetFlag(Flag::I, true);

    // get IRQ vector
    uint8_t IRQVectorL = Read(0xFFFE);
    uint8_t IRQVectorH = Read(0xFFFF);

    uint16_t IRQVector = (uint16_t)IRQVectorH << 8U | IRQVectorL;

    // jump to IRQ vector
    PC = IRQVector;
}

bool CPU::InstructionComplete()
{
    return currentInstructionCycles == 0;
}

void CPU::Clock()
{
    // CPU is processing the instruction
    if (currentInstructionCycles)
    {
        currentInstructionCycles--;
        return;
    }

    if (NMIPending)
    {
        NMIHandler();
        NMIPending = false;

        return;
    }

    if (IRQPending)
    {
        //if (GetFlag(Flag::I))
        //    return;

        IRQHandler();
        IRQPending = false;

        return;
    }

    opcode = Read(PC++);                                   // FETCH opcode and increment Program Counter

    Instruction instruction = instructionTable[opcode];    // DECODE instruction

    currentInstructionCycles = instruction.CPUcycles;      // set instruction cycles (instructions can add cycles)

    (this->*instruction.addressingMode)();                 // get address 
    (this->*instruction.instructionHandler)();             // EXECUTE instructions (add cycles if necessary)    

    if (pageCrossed)
        pageCrossed = false;
    
    totalCycles += currentInstructionCycles;

    currentInstructionCycles--;
}

/**** addressing modes ****/
/**** fixed (direct) addressing modes ****/
void CPU::Addr_Accumulator()
{
    // not used
}

void CPU::Addr_Implied()
{
    // not used
}

void CPU::Addr_Immediate()
{
    address = PC++;
}

void CPU::Addr_Absolute()
{
    uint8_t addressL = Read(PC++);
    uint8_t addressH = Read(PC++);

    address = (uint16_t)addressH << 8 | addressL;
}

void CPU::Addr_ZeroPage()
{
    address = (uint16_t)Read(PC++);
}

void CPU::Addr_Relative()
{
    uint8_t offset = Read(PC++);

    if (offset & 0x80)  // negative offset
    {
        offset = ~offset + 1;
        address = PC - offset;
    }
    else               // positive offset
        address = PC + (uint16_t)offset;

    if ((address & 0xFF00) != (PC & 0xFF00))
        pageCrossed = true;
}

/**** indexed addressing modes ****/ 
void CPU::Addr_AbsoluteIndexedX()
{
    uint8_t baseAddressL = Read(PC++);
    uint8_t baseAddressH = Read(PC++);

    uint16_t baseAddress = (uint16_t)baseAddressH << 8 | baseAddressL;

    address = baseAddress + (uint16_t)X;

    if ((address & 0xFF00) != (uint16_t)baseAddressH << 8)
        pageCrossed = true;
}

void CPU::Addr_AbsoluteIndexedY()
{
    uint8_t baseAddressL = Read(PC++);
    uint8_t baseAddressH = Read(PC++);

    uint16_t baseAddress = (uint16_t)baseAddressH << 8 | baseAddressL;

    address = baseAddress + (uint16_t)Y;

    if ((address & 0xFF00) != (uint16_t)baseAddressH << 8)
        pageCrossed = true;
}

void CPU::Addr_ZeroPageIndexedX()
{
    uint8_t baseAddress = Read(PC++);
    baseAddress += X;     // wrap around, same as: address = Read(PC++) + X & 0x00FF;

    address = (uint16_t)baseAddress;  // unsigned to wider unsigned: zero extended
}

void CPU::Addr_ZeroPageIndexedY()
{
    uint8_t baseAddress = Read(PC++);
    baseAddress += Y;    // wrap around, same as: address = Read(PC++) + Y & 0x00FF;

    address = (uint16_t)baseAddress;  // unsigned to wider unsigned: zero extended
}

/**** indirect addressing modes  ****/ 
void CPU::Addr_IndexedIndirect()
{
    uint8_t indirectAddress = Read(PC++);                            // pointer is in zero page 0x0000

    uint8_t baseAddressL = Read(indirectAddress + X & 0x00FF);       // wrap around
    uint8_t baseAddressH = Read(indirectAddress + X + 1 & 0x00FF);   // wrap around

    address = (uint16_t)baseAddressH << 8U | baseAddressL;
}

void CPU::Addr_IndirectIndexed()
{
    uint8_t indirectAddress = Read(PC++);                         // pointer is in zero page 0x0000

    uint8_t baseAddressL = Read(indirectAddress & 0x00FF);        // wrap around
    uint8_t baseAddressH = Read(indirectAddress + 1 & 0x00FF);    // wrap around

    uint16_t baseAddress = (uint16_t)baseAddressH << 8 | baseAddressL;

    address = baseAddress + Y;

    if ((address & 0xFF00) != (uint16_t)baseAddressH << 8)
        pageCrossed = true;
}

void CPU::Addr_AbsoluteIndirect()
{
    uint8_t indirectAddressL = Read(PC++);
    uint8_t indirectAddressH = Read(PC++);

    uint16_t indirectAddress = (uint16_t)indirectAddressH << 8U | indirectAddressL;

    uint8_t addressL = Read(indirectAddress);
    uint8_t addressH;
     
    if (indirectAddressL == 0xFF)   // bug
        addressH = Read(indirectAddress & 0xFF00);
    else
        addressH = Read(indirectAddress + 1);
    
    address = (uint16_t)addressH << 8U | addressL;   
}

/**** 6502 instructions ****/

// add memory to accumulator with carry
// - unsigned arithmetic:
//   the carry flag C is set if an overflow occurs (otherwise is cleared), allowing to chain 
//   more than one addition in order to sum numbers larger than 8 bits.
// - signed arithmetic:
//   numbers are considered in two-complement form, the overflow flag V is set if the result 
//   of the addition of two positive numbers is negative, or if the result of the addition of
//   two negative numbers is positive (meaning an overflow has occurred).
//   The sum of a positive and a negative number can never overflow.
void CPU::ADC()
{
    uint8_t data = Read(address);

    // sum accumulator + memory + carry bit
    uint16_t sum = A + data + P.bits.C;

    // save old accumulator (to check for overflow)
    uint8_t oldA = A; 

    //update accumulator
    A = (uint8_t)sum;     

    // update status flags
    SetFlag(Flag::C, sum & 0xFF00);
    SetFlag(Flag::V, (oldA ^ A) & ~(oldA ^ data) & 0x80);
    SetFlag(Flag::Z, A == 0);
    SetFlag(Flag::N, A & 0x80);

    if (pageCrossed)
        currentInstructionCycles++;
}

// logical AND accumulator with memory and store result in accumulator
void CPU::AND()
{
    uint8_t data = Read(address);

    // AND memory with accumulator and store in accumulator 
    A &= data;

    // update status flags
    SetFlag(Flag::N, A & 0x80);
    SetFlag(Flag::Z, A == 0x00);

    if (pageCrossed)
        currentInstructionCycles++;
}

// arithmetic shift left one bit (memory or accumulator)
void CPU::ASL()
{
    // operate on accumulator
    if (instructionTable[opcode].addressingMode == &CPU::Addr_Accumulator)
    {
        // shift accumulator one bit left (least significant bit == 0)
        uint8_t shifted = A << 1;

        // transfer most significant bit to carry flag and update other status flags
        SetFlag(Flag::C, A & 0x80);
        SetFlag(Flag::N, shifted & 0x80);
        SetFlag(Flag::Z, shifted == 0x00);

        A = shifted;
    }
    else   // operate on memory (read-modify-write)
    {
        // read data from memory
        uint8_t data = Read(address);

        // shift data one bit left (least significant bit == 0)
        uint8_t shifted = data << 1;

        // transfer most significant bit to carry flag and update other status flags
        SetFlag(Flag::C, data & 0x80);
        SetFlag(Flag::N, shifted & 0x80);
        SetFlag(Flag::Z, shifted == 0x00);

        data = shifted;

        // store data in memory
        Write(address, data);
    }
}

// branch on carry clear (C == 0)
void CPU::BCC()
{
    if (!GetFlag(Flag::C))
    {
        currentInstructionCycles++;      // if branch taken add cycle

        PC = address;

        if (pageCrossed)
            currentInstructionCycles++;  // if page crossed add cycle
    }
}

// branch on carry set (C == 1)
void CPU::BCS()
{
    if (GetFlag(Flag::C))
    {
        currentInstructionCycles++;      // if branch taken add cycle

        PC = address;

        if (pageCrossed)
            currentInstructionCycles++;  // if page crossed add cycle
    }
} 

// branch on result zero (Z == 1)
void CPU::BEQ()
{
    if (GetFlag(Flag::Z))
    {
        currentInstructionCycles++;      // if branch taken add cycle

        PC = address;

        if (pageCrossed)
            currentInstructionCycles++;  // if page crossed add cycle
    }
}

// test bits
void CPU::BIT()
{
    // test bits in memory with accumulator
    uint8_t data = Read(address);

    // AND accumulator and memory
    SetFlag(Flag::Z, (A & data) == 0);

    // transfer bits 6 and 7 of memory to status register 
    SetFlag(Flag::N, data & 0x80);
    SetFlag(Flag::V, data & 0x40);
}

// branch on result minus (N == 1)
void CPU::BMI()
{
    if (GetFlag(Flag::N))
    {
        currentInstructionCycles++;      // if branch taken add cycle

        PC = address;

        if (pageCrossed)
            currentInstructionCycles++;  // if page crossed add cycle
    }
}

// branch on result not zero (Z == 0)
void CPU::BNE()
{
    if (!GetFlag(Flag::Z))
    {
        currentInstructionCycles++;      // if branch taken add cycle

        PC = address;

        if (pageCrossed)
            currentInstructionCycles++;  // if page crossed add cycle
    }
}

// branch on result plus (N == 0)
void CPU::BPL()
{
    if (!GetFlag(Flag::N))
    {
        currentInstructionCycles++;      // if branch taken add cycle

        PC = address;

        if (pageCrossed)
            currentInstructionCycles++;  // if page crossed add cycle
    }
}

// force break
void CPU::BRK()
{
    // increment program counter
    PC++;

    // set break flag
    SetFlag(Flag::B, true);

    Write(stackBase + (uint16_t)S--, PC >> 8 & 0x00FF);     // push PCH on stack
    Write(stackBase + (uint16_t)S--, PC & 0x00FF);          // push PCL on stack
    Write(stackBase + (uint16_t)S--, P.reg);                // push processor status register on stack (B is set)

    // set interrupt disable flag
    SetFlag(Flag::I, true);

    // clear break flag
    SetFlag(Flag::B, false);

    // get IRQ vector
    uint8_t IRQVectorL = Read(0xFFFE);
    uint8_t IRQVectorH = Read(0xFFFF);

    uint16_t IRQVector = (uint16_t)IRQVectorH << 8 | IRQVectorL;

    // jump to IRQ vector
    PC = IRQVector;
}

// branch on overflow clear (V == 0)
void CPU::BVC()
{
    if (!GetFlag(Flag::V))
    {
        currentInstructionCycles++;      // if branch taken add cycle

        PC = address;

        if (pageCrossed)
            currentInstructionCycles++;  // if page crossed add cycle
    }
}

// brach on overflow set (V == 1)
void CPU::BVS()
{ 
    if (GetFlag(Flag::V))
    {
        currentInstructionCycles++;      // if branch taken add cycle

        PC = address;

        if (pageCrossed)
            currentInstructionCycles++;  // if page crossed add cycle
    }
}

// clear carry flag (C = 0)
void CPU::CLC()
{
    SetFlag(Flag::C, false);
}

// clear decimal mode flag (D = 0)
void CPU::CLD()
{
    SetFlag(Flag::D, false);
}

// clear interrupt disable flag (I = 0)
void CPU::CLI()
{
    SetFlag(Flag::I, false);
}

// clear overflow flag (V = 0)
void CPU::CLV()
{
    SetFlag(Flag::V, false);
}

// compare memory and accumulator
void CPU::CMP()
{
    uint8_t data = Read(address);

    // compare memory and accumulator (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    uint8_t result = A - data;

    // update status flags
    SetFlag(Flag::N, result & 0x80);
    SetFlag(Flag::Z, result == 0x00);
    SetFlag(Flag::C, A >= data);

    if (pageCrossed)
        currentInstructionCycles++;
}

// compare memory an index register X
void CPU::CPX()
{
    uint8_t data = Read(address);

    // compare data (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    uint8_t result = X - data;

    // update status flags
    SetFlag(Flag::N, result & 0x80);
    SetFlag(Flag::Z, result == 0x00);
    SetFlag(Flag::C, X >= data);
}

// compare memory and index register Y
void CPU::CPY()
{
    uint8_t data = Read(address);

    // compare data (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    uint8_t result = Y - data;

    // update status flags
    SetFlag(Flag::N, result & 0x80);
    SetFlag(Flag::Z, result == 0x00);
    SetFlag(Flag::C, Y >= data);
}

// decrement memory by one
void CPU::DEC()
{
    uint8_t data = Read(address);

    // decrement data (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    data--;

    // store decremented data in memory
    Write(address, data);

    // update status flags
    SetFlag(Flag::N, data & 0x80);
    SetFlag(Flag::Z, data == 0x00);
}

// decrement index register X by one
void CPU::DEX()
{
    // decrement index register X (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    X--;

    // update status flags
    SetFlag(Flag::N, X & 0x80);
    SetFlag(Flag::Z, X == 0x00);
}

// decrement index register Y by one
void CPU::DEY()
{
    // decrement index register Y (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    Y--;

    // update status flags
    SetFlag(Flag::N, Y & 0x80);
    SetFlag(Flag::Z, Y == 0x00);
}

// exclusive-OR memory with accumulator and store in accumulator
void CPU::EOR()
{
    uint8_t data = Read(address);

    // exclusive-OR memory with accumulator and store in accumulator
    A ^= data;

    // update status flags
    SetFlag(Flag::N, A & 0x80);
    SetFlag(Flag::Z, A == 0x00);

    if (pageCrossed)
        currentInstructionCycles++;
}

// increment memory by one
void CPU::INC()
{
    uint8_t data = Read(address);

    // increment data (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    data++;

    // store incremented data in memory
    Write(address, data);

    // update status flags
    SetFlag(Flag::N, data & 0x80);
    SetFlag(Flag::Z, data == 0x00);
}

// increment index register X by one 
void CPU::INX()
{
    // increment index register X (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    X++;

    // update status flags
    SetFlag(Flag::N, X & 0x80);
    SetFlag(Flag::Z, X == 0x00);
}

// increment index register Y by one 
void CPU::INY()
{
    // increment index register Y (note: C/C++ unsigned overflow/undeflow wraps around unsigned integers)
    Y++;

    // update status flags
    SetFlag(Flag::N, Y & 0x80);
    SetFlag(Flag::Z, Y == 0x00);
}

// unconditional jump to new location
void CPU::JMP()
{
    PC = address;
}

// jump to subroutine (saving return address)
void CPU::JSR()
{
    // save last byte of JSR instruction (return address) on the stack
    Write(stackBase + (uint16_t)S--, PC - 1 >> 8 & 0xFF);   // push PC high 
    Write(stackBase + (uint16_t)S--, PC - 1 & 0xFF);         // push PC low

    // jump to new address
    PC = address;
}

// load accumulator with memory
void CPU::LDA()
{
    A = Read(address);

    // update status flags
    SetFlag(Flag::N, A & 0x80);
    SetFlag(Flag::Z, A == 0x00);

    if (pageCrossed)
        currentInstructionCycles++;
}

// load index register X with memory
void CPU::LDX()
{
    X = Read(address);

    // update status flags
    SetFlag(Flag::N, X & 0x80);
    SetFlag(Flag::Z, X == 0x00);

    if (pageCrossed)
        currentInstructionCycles++;
}

// load index register Y with memory
void CPU::LDY()
{
    Y = Read(address);

    // update status flags
    SetFlag(Flag::N, Y & 0x80);
    SetFlag(Flag::Z, Y == 0x00);

    if (pageCrossed)
        currentInstructionCycles++;
}

// logical shift right one bit (memory or accumulator)
void CPU::LSR()
{
    // operate on accumulator
    if (instructionTable[opcode].addressingMode == &CPU::Addr_Accumulator)
    {
        // shift accumulator one bit right (most significant bit == 0)
        uint8_t shifted = A >> 1;

        // transfer least significant bit to carry flag and update other status flags
        SetFlag(Flag::C, A & 0x01);
        SetFlag(Flag::N, false);
        SetFlag(Flag::Z, shifted == 0x00);

        A = shifted;
    }
    else   // operate on memory (read-modify-write)
    {
        // read data from memory
        uint8_t data = Read(address);

        // shift data one bit right (most significant bit == 0)
        uint8_t shifted = data >> 1;

        // transfer least significant bit to carry flag and update other status flags
        SetFlag(Flag::C, data & 0x01);
        SetFlag(Flag::N, false);
        SetFlag(Flag::Z, shifted == 0x00);

        data = shifted;

        // store data in memory
        Write(address, data);
    }
}

// no operation
void CPU::NOP()
{
    // do nothing
}

// OR memory with accumulator and store in accumulator
void CPU::ORA()
{
    uint8_t data = Read(address);

    // OR memory with accumulator and store in accumulator
    A |= data;

    // update status flags
    SetFlag(Flag::N, A & 0x80);
    SetFlag(Flag::Z, A == 0x00);

    if (pageCrossed)
        currentInstructionCycles++;
}

// push accumulator on stack
void CPU::PHA()
{
    // push accumulator on stack, decrement stack pointer
    Write(stackBase + (uint16_t)S--, A);
}

// push processor status register on stack
void CPU::PHP()
{
    SetFlag(Flag::B, true);

    // push processor status register on stack, decrement stack pointer
    Write(stackBase + (uint16_t)S--, P.reg);

    SetFlag(Flag::B, false);
}

// pull (pop) from stack into accumulator
void CPU::PLA()
{
    // pull (pop) from stack into accumulator, increment stack pointer
    A = Read(stackBase + (uint16_t)++S);

    // update status flags
    SetFlag(Flag::N, A & 0x80);
    SetFlag(Flag::Z, A == 0x00);
}

// pull (pop) from stack into processor status register
void CPU::PLP()
{  
    bool oldB = GetFlag(Flag::B);

    // pull (pop) from stack into processor status register, increment stack pointer
    P.reg = Read(stackBase + (uint16_t)++S);
    
    SetFlag(Flag::B, oldB);
    SetFlag(Flag::UNUSED, true);
}

// rotate one bit left (memory or accumulator)
void CPU::ROL()
{
    // operate on accumulator
    if (instructionTable[opcode].addressingMode == &CPU::Addr_Accumulator)
    {
        // shift accumulator one bit left (least significant bit == carry flag)
        uint8_t shifted = A << 1 | P.bits.C;

        // transfer most significant bit to carry flag and update other status flags
        SetFlag(Flag::C, A & 0x80);
        SetFlag(Flag::N, shifted & 0x80);
        SetFlag(Flag::Z, shifted == 0x00);

        A = shifted;
    }
    else  // operate on memory (read-modify-write)
    {
        // read data from memory
        uint8_t data = Read(address);

        // shift data one bit left (least significant bit == carry flag)
        uint8_t shifted = data << 1U | P.bits.C;

        // transfer most significant bit to carry flag and update other status flags
        SetFlag(Flag::C, data & 0x80);
        SetFlag(Flag::N, shifted & 0x80);
        SetFlag(Flag::Z, shifted == 0x00);

        data = shifted;

        Write(address, data);
    }
}

// rotate one bit right (memory or accumulator)
void CPU::ROR()
{
    // operate on accumulator
    if (instructionTable[opcode].addressingMode == &CPU::Addr_Accumulator)
    {
        // shift accumulator one bit right (most significant bit == carry flag)
        uint8_t shifted = A >> 1U | P.bits.C << 7U;

        // transfer least significant bit to carry flag and update other status flags
        SetFlag(Flag::C, A & 0x01);
        SetFlag(Flag::N, shifted & 0x80);
        SetFlag(Flag::Z, shifted == 0x00);

        A = shifted;
    }
    else  // operate on memory (read-modify-write)
    {
        // read data from memory
        uint8_t data = Read(address);

        // shift data one bit right (most significant bit == carry flag)
        uint8_t shifted = data >> 1 | P.bits.C << 7;

        // transfer least significant bit to carry flag and update other status flags
        SetFlag(Flag::C, data & 0x01);
        SetFlag(Flag::N, shifted & 0x80);
        SetFlag(Flag::Z, shifted == 0x00);

        data = shifted;

        Write(address, data);
    }
}

// return from interrupt 
void CPU::RTI()
{
    // pop processor status register from stack
    P.reg = Read(stackBase + (uint16_t)++S);

    SetFlag(Flag::B, false);  // if interrupt was triggered by BRK instruction B is set
    SetFlag(Flag::UNUSED, true);

    // pop program counter low and high from stack
    uint8_t PCL = Read(stackBase + (uint16_t)++S);
    uint8_t PCH = Read(stackBase + (uint16_t)++S);

    // set program counter
    PC = (uint16_t)PCH << 8 | PCL;
}

// return from subroutine 
void CPU::RTS()
{
    uint8_t PCL = Read(stackBase + (uint16_t)++S);  // pop PC low
    uint8_t PCH = Read(stackBase + (uint16_t)++S);  // pop PC high

    // return to caller
    PC = ((uint16_t)PCH << 8 | PCL) + 1;   // add one to return address (was last byte of JSR instruction)
}

// subtract memory from accumulator with borrow
// - unsigned arithmetic:
//   
// - signed arithmetic:
//   
void CPU::SBC()
{
    uint8_t data = Read(address);

    // (one's) complement of data
    data = ~data;

    // sum accumulator + complement of memory + carry bit
    uint16_t difference = A + data + P.bits.C;

    // save old accumulator (to check for overflow)
    uint8_t oldA = A;

    //update accumulator
    A = (uint8_t)difference;

    // update status flags
    SetFlag(Flag::C, difference & 0xFF00);
    SetFlag(Flag::V, (oldA ^ A) & ~(oldA ^ data) & 0x80);
    SetFlag(Flag::Z, A == 0);
    SetFlag(Flag::N, A & 0x80);

    if (pageCrossed)
        currentInstructionCycles++;
}

// set carry flag (C = 1)
void CPU::SEC()
{
    SetFlag(Flag::C, true);
}

// set decimal mode flag (D = 1)
void CPU::SED()
{
    SetFlag(Flag::D, true);
}

// set interrupt disable flag (I = 1)
void CPU::SEI()
{
    SetFlag(Flag::I, true);
}

// store accumulator in memory
void CPU::STA()
{
    Write(address, A);
}

// store index register X in memory 
void CPU::STX()
{
    Write(address, X);
}

// store index register Y in memory
void CPU::STY()
{
    Write(address, Y);
}

// transfer accumulator to index register X
void CPU::TAX()
{
    X = A;

    // update status flags
    SetFlag(Flag::N, X & 0x80);
    SetFlag(Flag::Z, X == 0x00);
}

// transfer accumulator to index register Y
void CPU::TAY()
{
    Y = A;

    // update status flags
    SetFlag(Flag::N, Y & 0x80);
    SetFlag(Flag::Z, Y == 0x00);
}

// transfer stack pointer register to index register X
void CPU::TSX()
{
    X = S;

    // update status flags
    SetFlag(Flag::N, X & 0x80);
    SetFlag(Flag::Z, X == 0x00);
}

// transfer index register X to accumulator
void CPU::TXA()
{
    A = X;

    // update status flags
    SetFlag(Flag::N, A & 0x80);
    SetFlag(Flag::Z, A == 0x00);
}

// tranfer index register X to stack pointer register
void CPU::TXS()
{
    S = X;
}

// transfer index register Y to accumulator 
void CPU::TYA()
{
    A = Y;

    // update status flags
    SetFlag(Flag::N, A & 0x80);
    SetFlag(Flag::Z, A == 0x00);
}

std::map<uint16_t,std::string> CPU::Disassemble(int startAddress, int endAddress)
{
    std::map<uint16_t,std::string> assemblyCode;

    int address = startAddress;

    auto ToHexString = [](uint16_t num, uint8_t minDigits) -> std::string
                       { 
                            std::string hex;

                            do
                            {
                                hex += "0123456789ABCDEF"[num % 16];
                                minDigits--;
                            }
                            while (num /= 16);

                            while (minDigits--)
                                hex += '0';

                            for (int i = 0, j = hex.size() - 1; i < j; i++, j--)
                            {
                                char temp = hex[i];
                                hex[i] = hex[j];
                                hex[j] = temp;
                            }

                            return hex;
                       }; 

    while (address <= endAddress)
    {
        std::string codeLine;

        codeLine += "0x" + ToHexString(address, 4) + ": ";

        uint8_t opcode = Read(address++);

        Instruction instruction = instructionTable[opcode];

        codeLine += instruction.name + " ";

        if (instruction.addressingMode == &CPU::Addr_Accumulator)
        {
            codeLine += "{ACC}";
        }
        else if (instruction.addressingMode == &CPU::Addr_Implied)
        {
            codeLine += "{IMPL}";
        }
        else if (instruction.addressingMode == &CPU::Addr_Immediate)
        {
            codeLine += "0x" + ToHexString(Read(address), 2) + " ";
            codeLine += "{IMM}";

            address++;
        }
        else if (instruction.addressingMode == &CPU::Addr_Absolute)
        {
            codeLine += "0x" + ToHexString(Read(address + 1), 2) + ToHexString(Read(address), 2) + " ";
            codeLine += "{ABS}";
            
            address += 2;
        }
        else if (instruction.addressingMode == &CPU::Addr_ZeroPage)
        {
            codeLine += "0x" + ToHexString(Read(address), 2) + " ";
            codeLine += "{ZP}";

            address++;
        }
        else if (instruction.addressingMode == &CPU::Addr_Relative)
        {
            codeLine += std::to_string((int8_t)Read(address)) + " ";
            codeLine += "{REL}";

            address++;
        }
        else if (instruction.addressingMode == &CPU::Addr_AbsoluteIndexedX)
        {
            codeLine += "0x" + ToHexString(Read(address + 1), 2) + ToHexString(Read(address), 2) + ", X ";
            codeLine += "{ABX}";

            address += 2;
        }
        else if (instruction.addressingMode == &CPU::Addr_AbsoluteIndexedY)
        {
            codeLine += "0x" + ToHexString(Read(address + 1), 2) + ToHexString(Read(address), 2) + ", Y ";
            codeLine += "{ABY}";

            address += 2;
        }
        else if (instruction.addressingMode == &CPU::Addr_ZeroPageIndexedX)
        {
            codeLine += "0x" + ToHexString(Read(address), 2) + ", X ";
            codeLine += "{ZPX}";

            address++;
        }
        else if (instruction.addressingMode == &CPU::Addr_ZeroPageIndexedY)
        {
            codeLine += "0x" + ToHexString(Read(address), 2) + ", Y ";
            codeLine += "{ZPY}";

            address++;
        }
        else if (instruction.addressingMode == &CPU::Addr_IndexedIndirect)
        {
            codeLine += "(0x" + ToHexString(Read(address), 2) +  ToHexString(Read(address), 2) + ", X) ";
            codeLine += "{INX}";

            address++;
        }
        else if (instruction.addressingMode == &CPU::Addr_IndirectIndexed)
        {
            codeLine += "(0x" + ToHexString(Read(address), 2) +  ToHexString(Read(address), 2) + "), Y ";
            codeLine += "{INY}";

            address++;
        }
        else if (instruction.addressingMode == &CPU::Addr_AbsoluteIndirect)
        {
            codeLine += "(0x" + ToHexString(Read(address + 1), 2) + ToHexString(Read(address), 2) + ")  ";
            codeLine += "{IND}";
            
            address += 2;
        }

        assemblyCode[startAddress] = codeLine;
        startAddress = address;
    }

    return assemblyCode;
}