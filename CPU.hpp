#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>
#include <map>

class Bus;

// MOS6502 CPU 
class CPU
{
public:
    struct State
    {
        uint16_t PC;
        uint16_t A, X, Y, P, SP;
        uint64_t cycles;

        bool operator==(const State &other) { return PC == other.PC && A == other.A && X == other.X && Y == other.Y && SP == other.SP && P == other.P && cycles == other.cycles; }
        bool operator!=(const State &other) { return !(*this == other); }
    } state;

    State log[9000];
public:
    CPU(Bus &bus);

    /**** external inputs ****/
    void Reset();
    void NMI() { interruptPending = true; }
    void IRQ();

    void Clock();
    bool InstructionComplete();

    /**** debugging ****/
    std::map<uint16_t,std::string> Disassemble(int startAddress, int endAddress);
private:
    void NMIHandler();
private:
    /**** 6505 has 13 addressing modes: ****/
    // fixed/direct addressing modes
    void Addr_Accumulator(); // (1 byte - opcode)
    void Addr_Implied();     // (1 byte - opcode)
    void Addr_Immediate();   // (2 bytes - opcode + byte constant)
    void Addr_Absolute();    // (3 bytes - opcode + low address byte + high address byte (page))
    void Addr_ZeroPage();    // (2 bytes - opcode + low address byte (high address byte is always 0x00))
    void Addr_Relative();    // (2 bytes - opcode + relative address byte)
    // indexed addressing modes
    void Addr_AbsoluteIndexedX();  // (3 bytes - opcode + low base address byte + high base address byte (page), uses X index register)
    void Addr_AbsoluteIndexedY();  // (3 bytes - opcode + low base address byte + high base address byte (page), uses Y index register)
    void Addr_ZeroPageIndexedX();  // (2 bytes - opcode + low base address byte (high base address byte is always 0x00 - wraps around), uses X index register)
    void Addr_ZeroPageIndexedY();  // (2 bytes - opcode + low base address byte (high base address byte is always 0x00 - wraps around), uses Y index register)
    // indirect addressing modes 
    void Addr_IndexedIndirect();   // (2 bytes - opcode + low base address byte, uses X index register)
    void Addr_IndirectIndexed();   // (2 bytes - opcode + low base index byte, uses Y index register)
    void Addr_AbsoluteIndirect();  // (3 bytes - opcode + low base index byte + high base index byte)

    /**** 6502 has 56 instructions: ****/
    void ADC();   // add memory to accumulator with carry
    void AND();   // logical AND memory with accumulator and store result in accumulator
    void ASL();   // arithmetic shift left one bit (memory or accumulator)
    void BCC();   // branch on carry clear (C == 0)
    void BCS();   // branch on carry set (C == 1)
    void BEQ();   // branch on result zero (Z == 1)
    void BIT();   // test bits in memory with accumulator
    void BMI();   // branch on result minus (N == 1)
    void BNE();   // branch on result not zero (Z == 0)
    void BPL();   // branch on result plus (N == 0)
    void BRK();   // force break
    void BVC();   // branch on overflow clear (V == 0)
    void BVS();   // brach on overflow set (V == 1)
    void CLC();   // clear carry flag (C = 0)
    void CLD();   // clear decimal mode flag (D = 0)
    void CLI();   // clear interrupt disable flag (I = 0)
    void CLV();   // clear overflow flag (V = 0)
    void CMP();   // compare memory and accumulator
    void CPX();   // compare memory an index register X
    void CPY();   // compare memory and index register Y
    void DEC();   // decrement memory by one
    void DEX();   // decrement index register X by one
    void DEY();   // decrement index register Y by one
    void EOR();   // exclusive-OR memory with accumulator and store in accumulator
    void INC();   // increment memory by one
    void INX();   // increment index register X by one 
    void INY();   // increment index register Y by one
    void JMP();   // unconditional jump to new location
    void JSR();   // jump to subroutine (saving return address)
    void LDA();   // load accumulator with memory
    void LDX();   // load index register X with memory
    void LDY();   // load index register Y with memory
    void LSR();   // logical shift right one bit (memory or accumulator)
    void NOP();   // no operation
    void ORA();   // OR memory with accumulator and store in accumulator
    void PHA();   // push accumulator on stack
    void PHP();   // push processor status register on stack
    void PLA();   // pull (pop) from stack into accumulator
    void PLP();   // pull (pop) from stack into processor status register
    void ROL();   // rotate one bit left (memory or accumulator)
    void ROR();   // rotate one bit right (memory or accumulator)
    void RTI();   // return from interrupt 
    void RTS();   // return from subroutine 
    void SBC();   // subtract memory from accumulator with borrow
    void SEC();   // set carry flag (C = 1)
    void SED();   // set decimal mode flag (D = 1)
    void SEI();   // set interrupt disable flag (I = 1)
    void STA();   // store accumulator in memory
    void STX();   // store index register X in memory 
    void STY();   // store index register Y in memory
    void TAX();   // transfer accumulator to index register X
    void TAY();   // transfer accumulator to index register Y
    void TSX();   // transfer stack pointer register to index register X
    void TXA();   // transfer index register X to accumulator
    void TXS();   // tranfer index register X to stack pointer register
    void TYA();   // transfer index register Y to accumulator 
private:
    /**** read from/write to bus ****/
    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

    /**** utility functions set/clear and get processor status flags ****/
    enum class Flag { C = 0, Z, I, D, B, UNUSED, V, N };
    void SetFlag(Flag flag, bool condition);
    bool GetFlag(Flag flag);

    /**** CPU registers ****/
    union 
    {
        struct 
        {
            uint8_t C : 1;
            uint8_t Z : 1; 
            uint8_t I : 1;
            uint8_t D : 1;
            uint8_t B : 1;
            uint8_t UNUSED : 1;
            uint8_t V : 1;
            uint8_t N : 1;
        } bits;
        uint8_t reg;        
    } P;                     // processor status register
    uint8_t A;               // accumulator
    uint8_t X;               // X index register
    uint8_t Y;               // Y index register
    uint8_t S;               // stack pointer register
    uint16_t PC;             // program counter 

    /**** instruction table ****/
    using AddressingMode = void (CPU::*)();
    using InstructionHandler = void (CPU::*)(void);
    struct Instruction
    {
        std::string name;
        AddressingMode addressingMode;
        InstructionHandler instructionHandler;
        uint8_t CPUcycles;
    } instructionTable[256];    // instructions LUT;

    static const uint16_t stackBase = 0x0100;   // stack memory base address
    uint8_t opcode = 0x00;                      // current opcode
    uint16_t address = 0x0000;                  // current address 
    uint16_t currentInstructionCycles = 0;      // remaining cycles for executing instruction
    uint64_t totalCycles = 0;                   // total number of CPU clock cycles
    bool pageCrossed = false;                   // true if a 256 bytes memory page has been crossed

    bool interruptPending = false;

    Bus &bus;
};

#endif  // CPU_H