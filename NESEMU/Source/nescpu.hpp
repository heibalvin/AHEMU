#ifndef NESCPU_HPP
#define NESCPU_HPP

#include <SDL3/SDL.h>
#include "nescomponent.hpp"

// Macros for mask manipulation
#define MASK_SET(var, mask)    ((var) |= (mask))
#define MASK_CLEAR(var, mask)  ((var) &= ~(mask))
#define MASK_TOGGLE(var, mask) ((var) ^= (mask))
#define MASK_CHECK_SET(var, mask) (((var) & (mask)) == (mask))
#define MASK_CHECK_CLEAR(var, mask) (((var) & (mask)) == 0)
#define MASK_ASSIGN(var, mask, condition) ((var) = ((var) & ~(mask)) | ((condition) ? (mask) : 0))

class NESEMU;

enum NESCPUFLAGS: Uint8 {
    CARRY_FLAG              = 0x01,
    ZERO_FLAG               = 0x02,
    INTERRUPT_DISABLE_FLAG  = 0x04,
    DECIMAL_MODE_FLAG       = 0x08,
    BREAK_COMMAND_FLAG      = 0x10,
    UNUSED_FLAG             = 0x20,
    OVERFLOW_FLAG           = 0x40,
    NEGATIVE_FLAG           = 0x80
};

enum NESCPUADDRMODE {
    IMPLIED,
    ACCUMULATOR,
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    RELATIVE,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDEXED_INDIRECT,
    INDIRECT_INDEXED
};

/**
 * @class NESCPU
 * @brief Emulates the MOS 6502 central processing unit w/ 4KB Work RAM (WRAM).
 */
class NESCPU : public NESComponent {
public:
    explicit NESCPU(NESEMU* emu);
    ~NESCPU();

    void powerOn() override;
    void reset() override;

    Uint8 read(Uint16 address) override;
    void write(Uint16 address, Uint8 value) override;
    
    /**
     * @brief Initialise all 256 opcodes into a NESOPCODE structure (and set to UNKNOWN if not implemented)
     */
    void opcodesInit();

    /**
     * @brief Emulator moves 1 CPU cycle step. Taking into account waitCycles and moving to nextPC.
     */
    void step() override;
    void fetch();
    void decode();
    void execute();

    void NMIInterrupt();
    void IRQInterrupt();

    Uint16 getAddressMode();

    // Access LDA STA LDX STX LDY STY
    void LDA();
    void STA();
    void LDX();
    void STX();
    void LDY();
    void STY();

    // Transfer TAX TXA TAY TYA
    void TAX();
    void TXA();
    void TAY();
    void TYA();

    // Arithmetic ADC SBC INC DEC INX DEX INY DEY
    void ADC();
    void SBC();
    void INC();
    void DEC();
    void INX();
    void DEX();
    void INY();
    void DEY();

    // Shift ASL LSR ROL ROR
    void ASL();
    void LSR();
    void ROL();
    void ROR();

    // Bitwise AND ORA EOR BIT
    void AND();
    void ORA();
    void EOR();
    void BIT();

    // Compare CMP CPX CPY
    void CMP();
    void CPX();
    void CPY();

    // Branch BCC BCS BEQ BNE BPL BMI BVC BVS
    void BCC();
    void BCS();
    void BEQ();
    void BNE();
    void BPL();
    void BMI();
    void BVC();
    void BVS();

    /**
     * @brief Emulated CPU Jump: JMP JSR RTS BRK RTI
     */
    void JMP();
    void JSR();
    void RTS();
    void BRK();
    void RTI();

    // Stack: PHA PLA PHP PLP TXS TSX
    void PHA();
    void PLA();
    void PHP();
    void PLP();
    void TXS();
    void TSX();

    // Flags: CLC SEC CLI SEI CLD SED CLV
    void CLC();
    void SEC();
    void CLI();
    void SEI();
    void CLD();
    void SED();
    void CLV();

private:
    friend class NESEMU;
    friend class NESBUS;
    friend class NESPPU;

    // Interrupt signal lines
    bool nmi_asserted = false;
    bool irq_asserted = false;

    // Work RAM (WRAM) management
    Uint8 *wram = NULL;             // Work RAM (WRAM) for the main memory
    Uint8 A = 0x00;                 // Accumulator
    Uint8 X = 0x00;                 // X Register
    Uint8 Y = 0x00;                 // Y Register
    Uint8 P = 0x00;                 // Processor Status
    Uint8 SP = 0x00;                // Stack Pointer
    Uint16 PC = 0x0000;             // Program Counter

    Uint16 nextPC = 0x0000;         // Next Program Counter
    Uint8 bytes[3] = {0};           // Buffer to hold the raw instruction bytes for debugging
    int waitCycles = 0;             // Cycles to wait for the current instruction to complete
    char debugger[80];              // Buffer for formatted debugging output
    char program[80];               // Buffer for formatted program bytes for debugging
    char mnemonic[80];              // Buffer for formatted mnemonic for debugging

    struct NESCPUOPCODE {
        const char* nmemonic;
        void (NESCPU::*operation)();
        NESCPUADDRMODE addrMode;
        int length;
        int cycles;
    };

    NESCPUOPCODE opcodes[256];      // Opcode lookup table for instruction decoding
    NESCPUOPCODE* opcode = NULL;    // Pointer to the currently executing opcode 
};

#endif /* NESCPU_HPP */