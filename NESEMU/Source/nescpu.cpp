#include "nescpu.hpp"
#include "nesemu.hpp"
#include "nesbus.hpp"

NESCPU::NESCPU(NESEMU* emu) : NESComponent(emu) {
    // Initialize WRAM
    wram = (Uint8 *)SDL_malloc(0x0800); // 2KB of WRAM
    SDL_memset(wram, 0, 0x0800); // Clear WRAM

    opcodesInit(); // Initialize the opcode lookup table
}

NESCPU::~NESCPU() {
    if (wram) {
        SDL_free(wram);
        wram = NULL;
    }
}

void NESCPU::powerOn() {
    SDL_Log("NESCPU: powerOn()...");
    
    // Reset CPU state, registers, and flags
    A = 0x00; // Clear Accumulator
    X = 0x00; // Clear X Register
    Y = 0x00; // Clear Y Register
    
    reset(); // Call reset to initialize PC, SP, and flags
}

void NESCPU::reset() {
    // Reset CPU state, registers, and flags
    // A, X, Y unchanged
    // C, Z, D, V, N flags unchanged

    MASK_SET(P, INTERRUPT_DISABLE_FLAG);        // Clear Processor Status
    SP = 0xFD;                                  // Hardware reset sets SP to 0xFD after dropping 3 bytes)
    
    SDL_memset(wram, 0, 0x0800);                // Clear WRAM

    // Read initial program counter address from the 16-bit Reset Vector (0xFFFC)
    PC = emu->bus->readWord(0xFFFC);
    nextPC = PC;

    SDL_Log("NESCPU: PC start address %04X", PC);
}

Uint8 NESCPU::read(Uint16 address) {
    // Treat incoming addresses here as targeting CPU WRAM space ($0000-$1FFF mirrored)
    return wram[address % 0x0800];
}

void NESCPU::write(Uint16 address, Uint8 value) {
    wram[address % 0x0800] = value;
}

void NESCPU::opcodesInit() {
    // TBD: Initialize the opcodes with the appropriate mnemonics, operation function pointers, and cycle counts for each of the 256 opcodes.
    for(int i = 0; i < 256; i++) {
        opcodes[i] = {"UNKNOWN", NULL, IMPLIED, 1, 1};      // Default to an invalid opcode
    }

    // Access LDA STA LDX STX LDY STY
    opcodes[0xA9] = {"LDA #%02X - Load A IMMEDIATE", &NESCPU::LDA, IMMEDIATE, 2, 2};
    opcodes[0xA5] = {"LDA %02X - Load A ZERO_PAGE", &NESCPU::LDA, ZERO_PAGE, 2, 3};
    opcodes[0xB5] = {"LDA %02X, X - Load A ZERO_PAGE_X", &NESCPU::LDA, ZERO_PAGE_X, 2, 4};
    opcodes[0xAD] = {"LDA %04X - Load A ABSOLUTE", &NESCPU::LDA, ABSOLUTE, 3, 4};
    opcodes[0xBD] = {"LDA %04X, X - Load A ABSOLUTE_X", &NESCPU::LDA, ABSOLUTE_X, 3, 4};
    opcodes[0xB9] = {"LDA %04X, Y - Load A ABSOLUTE_Y", &NESCPU::LDA, ABSOLUTE_Y, 3, 4};
    opcodes[0xA1] = {"LDA (%02X, X) - Load A INDEXED_INDIRECT", &NESCPU::LDA, INDEXED_INDIRECT, 2, 6};
    opcodes[0xB1] = {"LDA (%02X), Y - Load A INDIRECT_INDEXED", &NESCPU::LDA, INDIRECT_INDEXED, 2, 5};

    opcodes[0x85] = {"STA %02X - Store A ZERO_PAGE", &NESCPU::STA, ZERO_PAGE, 2, 3};
    opcodes[0x95] = {"STA %02X, X - Store A ZERO_PAGE_X", &NESCPU::STA, ZERO_PAGE_X, 2, 4};
    opcodes[0x8D] = {"STA %04X - Store A ABSOLUTE", &NESCPU::STA, ABSOLUTE, 3, 4};
    opcodes[0x9D] = {"STA %04X, X - Store A ABSOLUTE_X", &NESCPU::STA, ABSOLUTE_X, 3, 5};
    opcodes[0x99] = {"STA %04X, Y - Store A ABSOLUTE_Y", &NESCPU::STA, ABSOLUTE_Y, 3, 5};
    opcodes[0x81] = {"STA (%02X, X) - Store A INDEXED_INDIRECT", &NESCPU::STA, INDEXED_INDIRECT, 2, 6};
    opcodes[0x91] = {"STA (%02X), Y - Store A INDIRECT_INDEXED", &NESCPU::STA, INDIRECT_INDEXED, 2, 6};

    opcodes[0xA2] = {"LDX #%02X - Load X IMMEDIATE", &NESCPU::LDX, IMMEDIATE, 2, 2};
    opcodes[0xA6] = {"LDX %02X - Load X ZERO_PAGE", &NESCPU::LDX, ZERO_PAGE, 2, 3};
    opcodes[0xB6] = {"LDX %02X, Y - Load X ZERO_PAGE_X", &NESCPU::LDX, ZERO_PAGE_Y, 2, 4};
    opcodes[0xAE] = {"LDX %04X - Load X ABSOLUTE", &NESCPU::LDX, ABSOLUTE, 3, 4};
    opcodes[0xBE] = {"LDX %04X, Y - Load X ABSOLUTE_Y", &NESCPU::LDX, ABSOLUTE_Y, 3, 4};

    opcodes[0x86] = {"STX %02X - Store X ZERO_PAGE", &NESCPU::STX, ZERO_PAGE, 2, 3};
    opcodes[0x96] = {"STX %02X, Y - Store X ZERO_PAGE_Y", &NESCPU::STX, ZERO_PAGE_Y, 2, 4};
    opcodes[0x8E] = {"STX %04X - Store X ABSOLUTE", &NESCPU::STX, ABSOLUTE, 3, 4};

    opcodes[0xA0] = {"LDY #%02X - Load Y IMMEDIATE", &NESCPU::LDY, IMMEDIATE, 2, 2};
    opcodes[0xA4] = {"LDY %02X - Load Y ZERO_PAGE", &NESCPU::LDY, ZERO_PAGE, 2, 3};
    opcodes[0xB4] = {"LDY %02X, X - Load Y ZERO_PAGE_X", &NESCPU::LDY, ZERO_PAGE_X, 2, 4};
    opcodes[0xAC] = {"LDY %04X - Load Y ABSOLUTE", &NESCPU::LDY, ABSOLUTE, 3, 4};
    opcodes[0xBC] = {"LDY %04X, X - Load Y ZERO_PAGE_X", &NESCPU::LDY, ABSOLUTE_X, 3, 4};

    opcodes[0x84] = {"STY %02X - Store Y ZERO_PAGE", &NESCPU::STY, ZERO_PAGE, 2, 3};
    opcodes[0x94] = {"STY %02X, X - Store Y ZERO_PAGE_X", &NESCPU::STY, ZERO_PAGE_X, 2, 4};
    opcodes[0x8C] = {"STY %04X - Store Y ABSOLUTE", &NESCPU::STY, ABSOLUTE, 3, 4};

    // Transfer TAX TXA TAY TYA
    opcodes[0xAA] = {"TAX - Transfer A to X IMPLIED", &NESCPU::TAX, IMPLIED, 1, 2};
    opcodes[0x8A] = {"TXA - Transfer X to A IMPLIED", &NESCPU::TXA, IMPLIED, 1, 2};
    opcodes[0xA8] = {"TAY - Transfer A to Y IMPLIED", &NESCPU::TAY, IMPLIED, 1, 2};
    opcodes[0x98] = {"TYA - Transfer Y to A IMPLIED", &NESCPU::TYA, IMPLIED, 1, 2};

    // Arithmetic ADC SBC INC DEC INX DEX INY DEY
    opcodes[0x69] = {"ADC %02X - Add with Carry IMMEDIATE", &NESCPU::ADC, IMMEDIATE, 2, 2};
    opcodes[0x65] = {"ADC (%02X) - Add with Carry ZERO_PAGE", &NESCPU::ADC, ZERO_PAGE, 2, 3};
    opcodes[0x75] = {"ADC (%02X), X - Add with Carry ZERO_PAGE_X", &NESCPU::ADC, ZERO_PAGE_X, 2, 4};
    opcodes[0x6D] = {"ADC %04X - Add with Carry ABSOLUTE", &NESCPU::ADC, ABSOLUTE, 3, 4};
    opcodes[0x7D] = {"ADC %04X, X - Add with Carry ABSOLUTE_X", &NESCPU::ADC, ABSOLUTE_X, 3, 4};
    opcodes[0x79] = {"ADC %04X, Y - Add with Carry ABSOLUTE_Y", &NESCPU::ADC, ABSOLUTE_Y, 3, 4};
    opcodes[0x61] = {"ADC (%04X, X) - Add with Carry INDEXED_INDIRECT", &NESCPU::ADC, INDEXED_INDIRECT, 2, 6};
    opcodes[0x71] = {"ADC (%04X), Y - Add with Carry INDIRECT_INDEXED", &NESCPU::ADC, INDIRECT_INDEXED, 2, 5};

    opcodes[0xE9] = {"SBC %02X - Subtract with Carry IMMEDIATE", &NESCPU::SBC, IMMEDIATE, 2, 2};
    opcodes[0xE5] = {"SBC (%02X) - Subtract with Carry ZERO_PAGE", &NESCPU::SBC, ZERO_PAGE, 2, 3};
    opcodes[0xF5] = {"SBC (%02X), X - Subtract with Carry ZERO_PAGE_X", &NESCPU::SBC, ZERO_PAGE_X, 2, 4};
    opcodes[0xED] = {"SBC %04X - Subtract with Carry ABSOLUTE", &NESCPU::SBC, ABSOLUTE, 3, 4};
    opcodes[0xFD] = {"SBC %04X, X - Subtract with Carry ABSOLUTE_X", &NESCPU::SBC, ABSOLUTE_X, 3, 4};
    opcodes[0xF9] = {"SBC %04X, Y - Subtract with Carry ABSOLUTE_Y", &NESCPU::SBC, ABSOLUTE_Y, 3, 4};
    opcodes[0xE1] = {"SBC (%02X, X) - Subtract with Carry INDEXED_INDIRECT", &NESCPU::SBC, INDEXED_INDIRECT, 2, 6};
    opcodes[0xF1] = {"SBC (%02X), Y - Subtract with Carry INDIRECT_INDEXED", &NESCPU::SBC, INDIRECT_INDEXED, 2, 5};

    opcodes[0xE6] = {"INC (%02X) - Increment Memory ZERO_PAGE", &NESCPU::INC, ZERO_PAGE, 2, 5};
    opcodes[0xF6] = {"INC (%02X), X - Increment Memory ZERO_PAGE_X", &NESCPU::INC, ZERO_PAGE_X, 2, 6};
    opcodes[0xEE] = {"INC %04X - Increment Memory ABSOLUTE", &NESCPU::INC, ABSOLUTE, 3, 6};
    opcodes[0xFE] = {"INC %04X, X - Increment Memory ABSOLUTE_X", &NESCPU::INC, ABSOLUTE_X, 3, 7};

    opcodes[0xC6] = {"DEC (%02X) - Decrement Memory ZERO_PAGE", &NESCPU::DEC, ZERO_PAGE, 2, 5};
    opcodes[0xD6] = {"DEC (%02X), X - Decrement Memory ZERO_PAGE_X", &NESCPU::DEC, ZERO_PAGE_X, 2, 6};
    opcodes[0xCE] = {"DEC %04X - Decrement Memory ABSOLUTE", &NESCPU::DEC, ABSOLUTE, 3, 6};
    opcodes[0xDE] = {"DEC %04X, X - Decrement Memory ABSOLUTE_X", &NESCPU::DEC, ABSOLUTE_X, 3, 7};

    opcodes[0xE8] = {"INX - Increment X Register", &NESCPU::INX, IMPLIED, 1, 2};

    opcodes[0xCA] = {"DEX - Decrement X Register", &NESCPU::DEX, IMPLIED, 1, 2};

    opcodes[0xC8] = {"INY - Increment Y Register", &NESCPU::INY, IMPLIED, 1, 2};

    opcodes[0x88] = {"DEY - Decrement Y Register", &NESCPU::DEY, IMPLIED, 1, 2};

    //
    // Shift ASL LSR ROL ROR
    //
    
    // ASL - Arithmetic Shift Left
    opcodes[0x0A] = {"ASL A - Arithmetic Shift Left ACCUMULATOR", &NESCPU::ASL, ACCUMULATOR, 1, 2};
    opcodes[0x06] = {"ASL (%02X) - Arithmetic Shift Left ZERO_PAGE", &NESCPU::ASL, ZERO_PAGE, 2, 5};
    opcodes[0x16] = {"ASL (%02X), X - Arithmetic Shift Left ZERO_PAGE_X", &NESCPU::ASL, ZERO_PAGE_X, 2, 6};
    opcodes[0x0E] = {"ASL %04X - Arithmetic Shift Left ABSOLUTE", &NESCPU::ASL, ABSOLUTE, 3, 6};
    opcodes[0x1E] = {"ASL %04X, X - Arithmetic Shift Left ABSOLUTE_X", &NESCPU::ASL, ABSOLUTE_X, 3, 7};

    // LSR - Logical Shift Right
    opcodes[0x4A] = {"LSR A - Logical Shift Right ACCUMULATOR", &NESCPU::LSR, ACCUMULATOR, 1, 2};
    opcodes[0x46] = {"LSR (%02X) - Logical Shift Right ZERO_PAGE", &NESCPU::LSR, ZERO_PAGE, 2, 5};
    opcodes[0x56] = {"LSR (%02X), X - Logical Shift Right ZERO_PAGE_X", &NESCPU::LSR, ZERO_PAGE_X, 2, 6};
    opcodes[0x4E] = {"LSR %04X - Logical Shift Right ABSOLUTE", &NESCPU::LSR, ABSOLUTE, 3, 6};
    opcodes[0x5E] = {"LSR %04X, X - Logical Shift Right ABSOLUTE_X", &NESCPU::LSR, ABSOLUTE_X, 3, 7};

    // ROL - Rotate Left
    opcodes[0x2A] = {"ROL A - Rotate Left ACCUMULATOR", &NESCPU::ROL, ACCUMULATOR, 1, 2};
    opcodes[0x26] = {"ROL (%02X) - Rotate Left ZERO_PAGE", &NESCPU::ROL, ZERO_PAGE, 2, 5};
    opcodes[0x36] = {"ROL (%02X), X - Rotate Left ZERO_PAGE_X", &NESCPU::ROL, ZERO_PAGE_X, 2, 6};
    opcodes[0x2E] = {"ROL %04X - Rotate Left ABSOLUTE", &NESCPU::ROL, ABSOLUTE, 3, 6};
    opcodes[0x3E] = {"ROL %04X, X - Rotate Left ABSOLUTE_X", &NESCPU::ROL, ABSOLUTE_X, 3, 7};

    // ROR - Rotate Right
    opcodes[0x6A] = {"ROR A - Rotate Right ACCUMULATOR", &NESCPU::ROR, ACCUMULATOR, 1, 2};
    opcodes[0x66] = {"ROR (%02X) - Rotate Right ZERO_PAGE", &NESCPU::ROR, ZERO_PAGE, 2, 5};
    opcodes[0x76] = {"ROR (%02X), X - Rotate Right ZERO_PAGE_X", &NESCPU::ROR, ZERO_PAGE_X, 2, 6};
    opcodes[0x6E] = {"ROR %04X - Rotate Right ABSOLUTE", &NESCPU::ROR, ABSOLUTE, 3, 6};
    opcodes[0x7E] = {"ROR %04X, X - Rotate Right ABSOLUTE_X", &NESCPU::ROR, ABSOLUTE_X, 3, 7};

    //
    // Compare CMP CPX CPY
    //

    // CMP - Compare Accumulator
    opcodes[0xC9] = {"CMP %02X - Compare Accumulator IMMEDIATE", &NESCPU::CMP, IMMEDIATE, 2, 2};
    opcodes[0xC5] = {"CMP (%02X) - Compare Accumulator ZERO_PAGE", &NESCPU::CMP, ZERO_PAGE, 2, 3};
    opcodes[0xD5] = {"CMP (%02X), X - Compare Accumulator ZERO_PAGE_X", &NESCPU::CMP, ZERO_PAGE_X, 2, 4};
    opcodes[0xCD] = {"CMP %04X - Compare Accumulator ABSOLUTE", &NESCPU::CMP, ABSOLUTE, 3, 4};
    opcodes[0xDD] = {"CMP %04X, X - Compare Accumulator ABSOLUTE_X", &NESCPU::CMP, ABSOLUTE_X, 3, 4};
    opcodes[0xD9] = {"CMP %04X, Y - Compare Accumulator ABSOLUTE_Y", &NESCPU::CMP, ABSOLUTE_Y, 3, 4};
    opcodes[0xC1] = {"CMP (%02X, X) - Compare Accumulator INDEXED_INDIRECT", &NESCPU::CMP, INDEXED_INDIRECT, 2, 6};
    opcodes[0xD1] = {"CMP (%02X), Y - Compare Accumulator INDIRECT_INDEXED", &NESCPU::CMP, INDIRECT_INDEXED, 2, 5};

    // CPX - Compare X Register
    opcodes[0xE0] = {"CPX %02X - Compare X Register IMMEDIATE", &NESCPU::CPX, IMMEDIATE, 2, 2};
    opcodes[0xE4] = {"CPX (%02X) - Compare X Register ZERO_PAGE", &NESCPU::CPX, ZERO_PAGE, 2, 3};
    opcodes[0xEC] = {"CPX %04X - Compare X Register ABSOLUTE", &NESCPU::CPX, ABSOLUTE, 3, 4};

    // CPY - Compare Y Register
    opcodes[0xC0] = {"CPY %02X - Compare Y Register IMMEDIATE", &NESCPU::CPY, IMMEDIATE, 2, 2};
    opcodes[0xC4] = {"CPY (%02X) - Compare Y Register ZERO_PAGE", &NESCPU::CPY, ZERO_PAGE, 2, 3};
    opcodes[0xCC] = {"CPY %04X - Compare Y Register ABSOLUTE", &NESCPU::CPY, ABSOLUTE, 3, 4};

    // Branch BCC BCS BEQ BNE BPL BMI BVC BVS
    opcodes[0x90] = {"BCC %02X - Branch if Carry Clear RELATIVE", &NESCPU::BCC, RELATIVE, 2, 2};
    opcodes[0xB0] = {"BCS %02X - Branch if Carry Set RELATIVE", &NESCPU::BCS, RELATIVE, 2, 2};
    opcodes[0xF0] = {"BEQ %02X - Branch if Equal RELATIVE", &NESCPU::BEQ, RELATIVE, 2, 2};
    opcodes[0xD0] = {"BNE %02X - Branch if Not Equal RELATIVE", &NESCPU::BNE, RELATIVE, 2, 2};
    opcodes[0x10] = {"BPL %02X - Branch if Plus RELATIVE", &NESCPU::BPL, RELATIVE, 2, 2};
    opcodes[0x30] = {"BMI %02X - Branch if Minus RELATIVE", &NESCPU::BMI, RELATIVE, 2, 2};
    opcodes[0x50] = {"BVC %02X - Branch if Overflow Clear RELATIVE", &NESCPU::BVC, RELATIVE, 2, 2};
    opcodes[0x70] = {"BVS %02X - Branch if Overflow Set RELATIVE", &NESCPU::BVS, RELATIVE, 2, 2};

    //
    // Bitwise AND ORA EOR BIT
    //

    // AND - Bitwise AND with Accumulator
    opcodes[0x29] = {"AND %02X - Bitwise AND IMMEDIATE", &NESCPU::AND, IMMEDIATE, 2, 2};
    opcodes[0x25] = {"AND (%02X) - Bitwise AND ZERO_PAGE", &NESCPU::AND, ZERO_PAGE, 2, 3};
    opcodes[0x35] = {"AND (%02X), X - Bitwise AND ZERO_PAGE_X", &NESCPU::AND, ZERO_PAGE_X, 2, 4};
    opcodes[0x2D] = {"AND %04X - Bitwise AND ABSOLUTE", &NESCPU::AND, ABSOLUTE, 3, 4};
    opcodes[0x3D] = {"AND %04X, X - Bitwise AND ABSOLUTE_X", &NESCPU::AND, ABSOLUTE_X, 3, 4};
    opcodes[0x39] = {"AND %04X, Y - Bitwise AND ABSOLUTE_Y", &NESCPU::AND, ABSOLUTE_Y, 3, 4};
    opcodes[0x21] = {"AND (%02X, X) - Bitwise AND INDEXED_INDIRECT", &NESCPU::AND, INDEXED_INDIRECT, 2, 6};
    opcodes[0x31] = {"AND (%02X), Y - Bitwise AND INDIRECT_INDEXED", &NESCPU::AND, INDIRECT_INDEXED, 2, 5};

    // ORA - Bitwise OR with Accumulator
    opcodes[0x09] = {"ORA %02X - Bitwise OR IMMEDIATE", &NESCPU::ORA, IMMEDIATE, 2, 2};
    opcodes[0x05] = {"ORA (%02X) - Bitwise OR ZERO_PAGE", &NESCPU::ORA, ZERO_PAGE, 2, 3};
    opcodes[0x15] = {"ORA (%02X), X - Bitwise OR ZERO_PAGE_X", &NESCPU::ORA, ZERO_PAGE_X, 2, 4};
    opcodes[0x0D] = {"ORA %04X - Bitwise OR ABSOLUTE", &NESCPU::ORA, ABSOLUTE, 3, 4};
    opcodes[0x1D] = {"ORA %04X, X - Bitwise OR ABSOLUTE_X", &NESCPU::ORA, ABSOLUTE_X, 3, 4};
    opcodes[0x19] = {"ORA %04X, Y - Bitwise OR ABSOLUTE_Y", &NESCPU::ORA, ABSOLUTE_Y, 3, 4};
    opcodes[0x01] = {"ORA (%02X, X) - Bitwise OR INDEXED_INDIRECT", &NESCPU::ORA, INDEXED_INDIRECT, 2, 6};
    opcodes[0x11] = {"ORA (%02X), Y - Bitwise OR INDIRECT_INDEXED", &NESCPU::ORA, INDIRECT_INDEXED, 2, 5};

    // EOR - Exclusive OR with Accumulator
    opcodes[0x49] = {"EOR %02X - Exclusive OR IMMEDIATE", &NESCPU::EOR, IMMEDIATE, 2, 2};
    opcodes[0x45] = {"EOR (%02X) - Exclusive OR ZERO_PAGE", &NESCPU::EOR, ZERO_PAGE, 2, 3};
    opcodes[0x55] = {"EOR (%02X), X - Exclusive OR ZERO_PAGE_X", &NESCPU::EOR, ZERO_PAGE_X, 2, 4};
    opcodes[0x4D] = {"EOR %04X - Exclusive OR ABSOLUTE", &NESCPU::EOR, ABSOLUTE, 3, 4};
    opcodes[0x5D] = {"EOR %04X, X - Exclusive OR ABSOLUTE_X", &NESCPU::EOR, ABSOLUTE_X, 3, 4};
    opcodes[0x59] = {"EOR %04X, Y - Exclusive OR ABSOLUTE_Y", &NESCPU::EOR, ABSOLUTE_Y, 3, 4};
    opcodes[0x41] = {"EOR (%02X, X) - Exclusive OR INDEXED_INDIRECT", &NESCPU::EOR, INDEXED_INDIRECT, 2, 6};
    opcodes[0x51] = {"EOR (%02X), Y - Exclusive OR INDIRECT_INDEXED", &NESCPU::EOR, INDIRECT_INDEXED, 2, 5};

    // BIT - Bit Test
    opcodes[0x24] = {"BIT (%02X) - Bit Test ZERO_PAGE", &NESCPU::BIT, ZERO_PAGE, 2, 3};
    opcodes[0x2C] = {"BIT %04X - Bit Test ABSOLUTE", &NESCPU::BIT, ABSOLUTE, 3, 4};

    // Jump: JMP JSR RTS BRK RTI
    //
    opcodes[0x4C] = {"JMP %04X - Jump", &NESCPU::JMP, ABSOLUTE, 3, 3};
    opcodes[0x6C] = {"JMP (%04X) - Jump", &NESCPU::JMP, INDIRECT, 3, 5};
    opcodes[0x20] = {"JSR %04X - Jump to Subroutine", &NESCPU::JSR, ABSOLUTE, 3, 6};
    opcodes[0x60] = {"RTS - Return from Subroutine", &NESCPU::RTS, IMPLIED, 1, 6};
    opcodes[0x00] = {"BRK - Break (software IRQ)", &NESCPU::BRK, IMMEDIATE, 2, 7};     // TBC: could be considered 1 (.IMPLIED) or 2 bytes (.IMMEDIATE) depending on how the instruction is implemented.
    opcodes[0x40] = {"RTI - Return from Interrupt", &NESCPU::RTI, IMPLIED, 1, 6};

    // Stack: PHA PLA PHP PLP TXS TSX
    opcodes[0x48] = {"PHA - Push A", &NESCPU::PHA, IMPLIED, 1, 3};
    opcodes[0x68] = {"PLA - Pull A", &NESCPU::PLA, IMPLIED, 1, 3};
    opcodes[0x08] = {"PHP - Push Processor Status", &NESCPU::PHP, IMPLIED, 1, 3};
    opcodes[0x28] = {"PLP - Pull Processor Status", &NESCPU::PLP, IMPLIED, 1, 3};
    opcodes[0x9A] = {"TXS - Transfer X to Stack Pointer", &NESCPU::TXS, IMPLIED, 1, 2};
    opcodes[0xBA] = {"TSX - Transfer Stack Pointer to X", &NESCPU::TSX, IMPLIED, 1, 2};

    // Flags: CLC SEC CLI SEI CLD SED CLV
    opcodes[0x18] = {"CLC - Clear Carry", &NESCPU::CLC, IMPLIED, 1, 2};
    opcodes[0x38] = {"SEC - Set Carry", &NESCPU::SEC, IMPLIED, 1, 2};
    opcodes[0x58] = {"CLI - Clear Interrupt Disable", &NESCPU::CLI, IMPLIED, 1, 2};
    opcodes[0x78] = {"SEI - Set Interrupt Disable", &NESCPU::SEI, IMPLIED, 1, 2};
    opcodes[0xF8] = {"SED - Set Decimal", &NESCPU::SED, IMPLIED, 1, 2};
    opcodes[0xD8] = {"CLD - Clear Decimal", &NESCPU::CLD, IMPLIED, 1, 2};
    opcodes[0xB8] = {"CLV - Clear Overflow", &NESCPU::CLV, IMPLIED, 1, 2};

    // Other
    opcodes[0xEA] = {"NOP - No Operation", NULL, IMPLIED, 1, 2};
}

void NESCPU::step() {
    // 0. Decrement wait cycles if we're still waiting for the current instruction to complete
    if (waitCycles > 0) {
        waitCycles--;
        // Autonomously raise a raw cycle event if the user wants cycle-by-cycle debugging
        emu->raiseEvent(NESEvent::CYCLE_STEP);
        return; 
    }

    // --- Executed ONLY when waitCycles == 0 (Instruction Boundary) ---
    emu->raiseEvent(NESEvent::INSTRUCTION_STEP);

    // 1. Check for Non-Maskable Interrupt first (NMI takes priority)
    if (nmi_asserted) {
        nmi_asserted = false; // Acknowledge edge-triggered NMI
        NMIInterrupt();
        emu->raiseEvent(NESEvent::NMI_TRIGGERED);
        return; // Processing NMI takes up the entire instruction window cycle
    }

    // 2. Check for regular hardware IRQ (Respects the disable flag)
    if (irq_asserted && MASK_CHECK_CLEAR(P, INTERRUPT_DISABLE_FLAG)) {
        // Note: We do NOT clear irq_asserted here because level-sensitive IRQ lines 
        // stay low until the external device (like APU/Mapper) is explicitly acknowledged by code.
        IRQInterrupt();
        emu->raiseEvent(NESEvent::IRQ_TRIGGERED);
        return;
    }

    // 3. Normal execution flow if no hardware interrupts are hijacking the cycle
    fetch();
    decode();
    execute(); // sets PC and waitCycles for the next operation
}

void NESCPU::fetch() {
    opcode = &opcodes[emu->bus->read(PC)];  // Fetch the opcode from memory at the current PC
    
    for(int i = 0; i < opcode->length; i++) {
        bytes[i] = emu->bus->read(PC + i); // Fetch the raw instruction bytes for debugging
    }
}

void NESCPU::decode() {
    // TBD: Implement instruction decoding logic, which involves interpreting the fetched instruction and determining which operation to perform, as well as which registers or memory locations are involved.
    SDL_snprintf(debugger, sizeof(debugger), "A: %02X X: %02X Y: %02X SP: %02X P:%c%c%c%c%c%c%c", A, X, Y, SP,
                 (P & CARRY_FLAG) ? 'C' : '.',
                 (P & ZERO_FLAG) ? 'Z' : '.',
                 (P & INTERRUPT_DISABLE_FLAG) ? 'I' : '.',
                 (P & DECIMAL_MODE_FLAG) ? 'D' : '.',
                 (P & BREAK_COMMAND_FLAG) ? 'B' : '.',
                 (P & OVERFLOW_FLAG) ? 'V' : '.',
                 (P & NEGATIVE_FLAG) ? 'N' : '.'); // Format the register values and flags for debugging output

    if (opcode->length == 1) {
        SDL_snprintf(program, sizeof(program), "PC: %04X | %02X .. ..", PC, bytes[0]); 
        SDL_snprintf(mnemonic, sizeof(mnemonic), "%s", opcode->nmemonic);
    } else if (opcode->length == 2) {
        SDL_snprintf(program, sizeof(program), "PC: %04X | %02X %02X ..", PC, bytes[0], bytes[1]);
        SDL_snprintf(mnemonic, sizeof(mnemonic), opcode->nmemonic, bytes[1]);
    } else if (opcode->length == 3) {
        SDL_snprintf(program, sizeof(program), "PC: %04X | %02X %02X %02X", PC, bytes[0], bytes[1], bytes[2]);
        SDL_snprintf(mnemonic, sizeof(mnemonic), opcode->nmemonic, Uint16(bytes[2]) << 8 | Uint16(bytes[1]));
    }

    // SDL_Log("%s | %s | %s", debugger, program, mnemonic); // Log the current CPU state, program bytes, and mnemonic for debugging
}

void NESCPU::execute() {
    // Increment the Program Counter to point to the next instruction 
    nextPC = PC + opcode->length;

    // Set the wait cycles for the current instruction (subtract 1 because we will execute the next instruction on the next step)
    waitCycles = opcode->cycles - 1;
    
    // Call the operation function pointer for the current opcode
    if (opcode->operation) {
        (this->*opcode->operation)();
    }

    // Update the Program Counter to the next instruction after execution
    PC = nextPC;
}

void NESCPU::NMIInterrupt() {
    wram[0x0100 + SP] = (PC >> 8) & 0xFF;
    wram[0x0100 + SP - 1] = PC & 0xFF;
    SP -= 2;

    Uint8 statusToPush = (P & ~BREAK_COMMAND_FLAG) | UNUSED_FLAG;
    wram[0x0100 + SP] = statusToPush;
    SP -= 1;

    MASK_SET(P, INTERRUPT_DISABLE_FLAG);

    PC = emu->bus->readWord(0xFFFA);
    nextPC = PC;
    
    waitCycles = 7;
}

void NESCPU::IRQInterrupt() {
    wram[0x0100 + SP] = (PC >> 8) & 0xFF;
    wram[0x0100 + SP - 1] = PC & 0xFF;
    SP -= 2;

    Uint8 statusToPush = (P & ~BREAK_COMMAND_FLAG) | UNUSED_FLAG;
    wram[0x0100 + SP] = statusToPush;
    SP -= 1;

    MASK_SET(P, INTERRUPT_DISABLE_FLAG);

    PC = emu->bus->readWord(0xFFFE);
    nextPC = PC;
    
    waitCycles = 7;
}

//
// Helper functions
//

Uint16 NESCPU::getAddressMode() {
    switch (opcode->addrMode) {
        case IMMEDIATE:
            return PC + 1;
            break;
        case ZERO_PAGE:
            return Uint16(bytes[1]); 
            break;
        case ZERO_PAGE_X:
            return (Uint16(bytes[1]) + X) & 0x00FF;
            break;
        case ZERO_PAGE_Y:
            return (Uint16(bytes[1]) + Y) & 0x00FF;
            break;
        case ABSOLUTE:
            return (Uint16(bytes[2]) << 8 | Uint16(bytes[1]));
            break;
        case ABSOLUTE_X:
            return (Uint16(bytes[2]) << 8 | Uint16(bytes[1])) + Uint16(X);
            break;
        case ABSOLUTE_Y:
            return (Uint16(bytes[2]) << 8 | Uint16(bytes[1])) + Uint16(Y);
            break;
        case INDIRECT: {
            // Read the 16-bit vector target address from instruction bytes
            Uint16 vector = (Uint16(bytes[2]) << 8) | Uint16(bytes[1]);
            return emu->bus->readWord(vector);
        }
        case INDEXED_INDIRECT: {
            // (d,X) mode: address is fetched from zero-page pointer (bytes[1] + X)
            Uint8 zeroPageAddr = bytes[1] + X; // Intentionally wraps around 8-bit space
            return emu->bus->readWord(zeroPageAddr);
        }
        case INDIRECT_INDEXED: {
            // (d),Y mode: base address from zero-page pointer, then add Y index
            Uint16 base = emu->bus->readWord(bytes[1]);
            return base + Uint16(Y);
        }
        default:
            break;
    }
    return 0x0000;
}

//
// Access LDA STA LDX STX LDY STY
//
void NESCPU::LDA() {
    Uint16 addr = getAddressMode();
    A = emu->bus->read(addr);
    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

void NESCPU::STA() {
    Uint16 addr = getAddressMode();
    emu->bus->write(addr, A);
}

void NESCPU::LDX() {
    Uint16 addr = getAddressMode();
    X = emu->bus->read(addr);
    MASK_ASSIGN(P, ZERO_FLAG, X == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (X & 0x80) != 0);
}

void NESCPU::STX() {
    Uint16 addr = getAddressMode();
    emu->bus->write(addr, X);
}

void NESCPU::LDY() {
    Uint16 addr = getAddressMode();
    Y = emu->bus->read(addr);
    MASK_ASSIGN(P, ZERO_FLAG, Y == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (Y & 0x80) != 0);
}

void NESCPU::STY() {
    Uint16 addr = getAddressMode();
    emu->bus->write(addr, Y);
}

//
// Transfer TAX TXA TAY TYA
//
void NESCPU::TAX() {
    X = A;
    MASK_ASSIGN(P, ZERO_FLAG, X == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (X & 0x80) != 0);
}

void NESCPU::TXA() {
    A = X;
    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

void NESCPU::TAY() {
    Y = A;
    MASK_ASSIGN(P, ZERO_FLAG, Y == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (Y & 0x80) != 0);
}

void NESCPU::TYA() {
    A = Y;
    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

//
// Arithmetic ADC SBC INC DEC INX DEX INY DEY
//
void NESCPU::ADC() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);
    
    Uint16 value = Uint16(A) + Uint16(byte) + (MASK_CHECK_SET(P, CARRY_FLAG) ? 1 : 0);

    // Carry Flag: Set if value exceeds 8-bit bounds
    MASK_ASSIGN(P, CARRY_FLAG, value > 0x00FF);

    // Overflow Flag: Check if sign bit flipped during addition
    bool hasOverflow = (~(Uint16(A) ^ Uint16(byte)) & (Uint16(A) ^ value)) & 0x0080;
    MASK_ASSIGN(P, OVERFLOW_FLAG, hasOverflow);

    A = Uint8(value & 0x00FF);
    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

void NESCPU::SBC() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);

    // Invert the operand for two's complement addition
    Uint8 inverted_byte = ~byte;

    Uint16 value = Uint16(A) + Uint16(inverted_byte) + (MASK_CHECK_SET(P, CARRY_FLAG) ? 1 : 0);

    // Carry flag acts as a "not-borrow" bit in SBC
    MASK_ASSIGN(P, CARRY_FLAG, value > 0x00FF);

    // Overflow logic matches ADC but checks against the inverted operand
    bool hasOverflow = (~(Uint16(A) ^ Uint16(inverted_byte)) & (Uint16(A) ^ value)) & 0x0080;
    MASK_ASSIGN(P, OVERFLOW_FLAG, hasOverflow);

    A = Uint8(value & 0x00FF);
    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

void NESCPU::INC() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);
    
    byte++;
    emu->bus->write(addr, byte);
    
    MASK_ASSIGN(P, ZERO_FLAG, byte == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (byte & 0x80) != 0);
}

void NESCPU::DEC() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);
    
    byte--;
    emu->bus->write(addr, byte);
    
    MASK_ASSIGN(P, ZERO_FLAG, byte == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (byte & 0x80) != 0);
}

void NESCPU::INX() {
    // Increment the X register (handles 8-bit wrap-around automatically)
    X++;
    
    // Update Zero and Negative flags based on the new state of X
    MASK_ASSIGN(P, ZERO_FLAG, X == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (X & 0x80) != 0);
}

void NESCPU::DEX() {
    // Decrement the X register
    X--;
    
    // Update Zero and Negative flags based on the new state of X
    MASK_ASSIGN(P, ZERO_FLAG, X == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (X & 0x80) != 0);
}

void NESCPU::INY() {
    // Increment the Y register
    Y++;
    
    // Update Zero and Negative flags based on the new state of Y
    MASK_ASSIGN(P, ZERO_FLAG, Y == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (Y & 0x80) != 0);
}

void NESCPU::DEY() {
    // Decrement the Y register
    Y--;
    
    // Update Zero and Negative flags based on the new state of Y
    MASK_ASSIGN(P, ZERO_FLAG, Y == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (Y & 0x80) != 0);
}

//
// Shift ASL LSR ROL ROR
//
void NESCPU::ASL() {
    Uint16 addr = 0;
    Uint8 data = 0;

    if (opcode->addrMode == ACCUMULATOR) {
        data = A;
    } else {
        addr = getAddressMode();
        data = emu->bus->read(addr);
    }

    // Carry gets bit 7 of the target data before the shift
    MASK_ASSIGN(P, CARRY_FLAG, (data & 0x80) != 0);

    // Perform shift left
    data <<= 1;

    // Evaluate Zero and Negative Flags
    MASK_ASSIGN(P, ZERO_FLAG, data == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (data & 0x80) != 0);

    if (opcode->addrMode == ACCUMULATOR) {
        A = data;
    } else {
        emu->bus->write(addr, data);
    }
}

void NESCPU::LSR() {
    Uint16 addr = 0;
    Uint8 data = 0;

    if (opcode->addrMode == ACCUMULATOR) {
        data = A;
    } else {
        addr = getAddressMode();
        data = emu->bus->read(addr);
    }

    // Carry gets bit 0 of the target data before the shift
    MASK_ASSIGN(P, CARRY_FLAG, (data & 0x01) != 0);

    // Perform logical shift right (0 is pushed into bit 7)
    data >>= 1;

    // Evaluate Zero and Negative Flags
    MASK_ASSIGN(P, ZERO_FLAG, data == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (data & 0x80) != 0);

    if (opcode->addrMode == ACCUMULATOR) {
        A = data;
    } else {
        emu->bus->write(addr, data);
    }
}

void NESCPU::ROL() {
    Uint16 addr = 0;
    Uint8 data = 0;

    if (opcode->addrMode == ACCUMULATOR) {
        data = A;
    } else {
        addr = getAddressMode();
        data = emu->bus->read(addr);
    }

    // Store the old carry bit to push into bit 0
    Uint8 old_carry = MASK_CHECK_SET(P, CARRY_FLAG) ? 1 : 0;

    // New carry bit gets bit 7 of the data boundary
    MASK_ASSIGN(P, CARRY_FLAG, (data & 0x80) != 0);

    // Rotate: shift left and place the old carry into the lowest bit
    data = (data << 1) | old_carry;

    MASK_ASSIGN(P, ZERO_FLAG, data == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (data & 0x80) != 0);

    if (opcode->addrMode == ACCUMULATOR) {
        A = data;
    } else {
        emu->bus->write(addr, data);
    }
}

void NESCPU::ROR() {
    Uint16 addr = 0;
    Uint8 data = 0;

    if (opcode->addrMode == ACCUMULATOR) {
        data = A;
    } else {
        addr = getAddressMode();
        data = emu->bus->read(addr);
    }

    // Store the old carry bit to push into bit 7
    Uint8 old_carry = MASK_CHECK_SET(P, CARRY_FLAG) ? 0x80 : 0x00;

    // New carry bit gets bit 0 of the data boundary
    MASK_ASSIGN(P, CARRY_FLAG, (data & 0x01) != 0);

    // Rotate: shift right and place the old carry into the highest bit
    data = (data >> 1) | old_carry;

    MASK_ASSIGN(P, ZERO_FLAG, data == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (data & 0x80) != 0);

    if (opcode->addrMode == ACCUMULATOR) {
        A = data;
    } else {
        emu->bus->write(addr, data);
    }
}

//
// Bitwise AND ORA EOR BIT
//

void NESCPU::AND() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);

    A &= byte;

    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

void NESCPU::ORA() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);

    A |= byte;

    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

void NESCPU::EOR() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);

    A ^= byte;

    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

void NESCPU::BIT() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);

    // 1. Zero Flag is set based on the result of a bitwise AND between A and memory byte
    MASK_ASSIGN(P, ZERO_FLAG, (A & byte) == 0);

    // 2. Negative Flag gets bit 7 of the memory byte directly
    MASK_ASSIGN(P, NEGATIVE_FLAG, (byte & 0x80) != 0);

    // 3. Overflow Flag gets bit 6 of the memory byte directly
    MASK_ASSIGN(P, OVERFLOW_FLAG, (byte & 0x40) != 0);
}

//
// Branch BCC BCS BEQ BNE BPL BMI BVC BVS
//
void NESCPU::BCC() {
    if MASK_CHECK_CLEAR(P, CARRY_FLAG) {
        Sint8 offset = static_cast<Sint8>(bytes[1]);
        nextPC += offset;
    }
}

void NESCPU::BCS() {
    if MASK_CHECK_SET(P, CARRY_FLAG) {
        Sint8 offset = static_cast<Sint8>(bytes[1]);
        nextPC += offset;
    }
}

void NESCPU::BEQ() {
    if MASK_CHECK_SET(P, ZERO_FLAG) {
        Sint8 offset = static_cast<Sint8>(bytes[1]);
        nextPC += offset;
    }
}

void NESCPU::BNE() {
    if MASK_CHECK_CLEAR(P, ZERO_FLAG) {
        Sint8 offset = static_cast<Sint8>(bytes[1]);
        nextPC += offset;
    }
}

void NESCPU::BPL() {
    if MASK_CHECK_CLEAR(P, NEGATIVE_FLAG) {
        Sint8 offset = static_cast<Sint8>(bytes[1]);
        nextPC += offset;
    }
}

void NESCPU::BMI() {
    if MASK_CHECK_SET(P, NEGATIVE_FLAG) {
        Sint8 offset = static_cast<Sint8>(bytes[1]);
        nextPC += offset;
    }
}

void NESCPU::BVC() {
    if MASK_CHECK_CLEAR(P, OVERFLOW_FLAG) {
        Sint8 offset = static_cast<Sint8>(bytes[1]);
        nextPC += offset;
    }    
}

void NESCPU::BVS() {
    if MASK_CHECK_SET(P, OVERFLOW_FLAG) {
        Sint8 offset = static_cast<Sint8>(bytes[1]);
        nextPC += offset;
    }
}

//
// Compare CMP CPX CPY
//

void NESCPU::CMP() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);

    // Virtual subtraction: A - byte
    Uint16 result = Uint16(A) - Uint16(byte);

    // Carry flag is set if A >= byte
    MASK_ASSIGN(P, CARRY_FLAG, A >= byte);
    
    // Zero flag is set if the comparison matches exactly
    MASK_ASSIGN(P, ZERO_FLAG, A == byte);
    
    // Negative flag tracks bit 7 of the temporary 8-bit result space
    MASK_ASSIGN(P, NEGATIVE_FLAG, (result & 0x0080) != 0);
}

void NESCPU::CPX() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);

    // Virtual subtraction: X - byte
    Uint16 result = Uint16(X) - Uint16(byte);

    // Carry flag is set if X >= byte
    MASK_ASSIGN(P, CARRY_FLAG, X >= byte);
    
    // Zero flag is set if the comparison matches exactly
    MASK_ASSIGN(P, ZERO_FLAG, X == byte);
    
    // Negative flag tracks bit 7 of the temporary 8-bit result space
    MASK_ASSIGN(P, NEGATIVE_FLAG, (result & 0x0080) != 0);
}

void NESCPU::CPY() {
    Uint16 addr = getAddressMode();
    Uint8 byte = emu->bus->read(addr);

    // Virtual subtraction: Y - byte
    Uint16 result = Uint16(Y) - Uint16(byte);

    // Carry flag is set if Y >= byte
    MASK_ASSIGN(P, CARRY_FLAG, Y >= byte);
    
    // Zero flag is set if the comparison matches exactly
    MASK_ASSIGN(P, ZERO_FLAG, Y == byte);
    
    // Negative flag tracks bit 7 of the temporary 8-bit result space
    MASK_ASSIGN(P, NEGATIVE_FLAG, (result & 0x0080) != 0);
}

//
// Jump: JMP JSR RTS BRK RTI
//

// JMP - Jump
void NESCPU::JMP() {
    nextPC = getAddressMode();
}

// JSR - Jump to Subroutine
void NESCPU::JSR() {
    Uint16 returnAddress = PC + 2;                      // Address of last byte of JSR instruction
    wram[0x0100 + SP] = (returnAddress >> 8) & 0xFF;    // Push High Byte
    wram[0x0100 + SP - 1] = returnAddress & 0xFF;       // Push Low Byte
    SP -= 2;
    nextPC = getAddressMode();
}

// RTS - Return from Subroutine
void NESCPU::RTS() {
    // Pull low byte and high byte in exact reverse alignment order
    Uint8 low = wram[0x0100 + SP + 1];
    Uint8 high = wram[0x0100 + SP + 2];
    SP += 2;
    
    // Combine addresses securely using parentheses, then step past last byte (+1)
    nextPC = ((Uint16(high) << 8) | low) + 1;
}

// BRK - Break (software IRQ)
void NESCPU::BRK() {
    wram[0x0100 + SP] = (PC >> 8) & 0xFF;                                   // Push high byte of PC
    wram[0x0100 + SP - 1] = PC & 0xFF;                                      // Push low byte of PC
    SP -= 2;
    PHP();                                                                  // Push Processor Status
    MASK_SET(P, INTERRUPT_DISABLE_FLAG);                                    // Set Interrupt Disable flag
    nextPC = emu->bus->readWord(0xFFFE);                                    // Jump to BRK vector address
}

// RTI - Return from Interrupt
void NESCPU::RTI() {
    PLP(); // Pulls the status flags, increments SP by 1
    
    // Pull PC Low Byte
    SP += 1;
    Uint8 low = wram[0x0100 + SP];
    
    // Pull PC High Byte
    SP += 1;
    Uint8 high = wram[0x0100 + SP];
    
    nextPC = (Uint16(high) << 8) | low;
}

//
// Stack: PHA PLA PHP PLP TXS TSX
//

// PHA - Push A
void NESCPU::PHA() {
    wram[0x0100 + SP] = A;
    SP -= 1;
}

// PLA - Pull A
void NESCPU::PLA() {
    SP += 1;
    A = wram[0x0100 + SP];
    
    MASK_ASSIGN(P, ZERO_FLAG, A == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (A & 0x80) != 0);
}

// PHP - Push Processor Status
void NESCPU::PHP() {
    wram[0x0100 + SP] = P | BREAK_COMMAND_FLAG | UNUSED_FLAG;
    SP -= 1;
}

// PLP - Pull Processor Status
void NESCPU::PLP() {
    SP += 1;
    P = wram[0x0100 + SP];
}

// TXS - Transfer X to Stack Pointer
void NESCPU::TXS() {
    SP = X;
}

// TSX - Transfer Stack Pointer to X
void NESCPU::TSX() {
    X = SP;
    MASK_ASSIGN(P, ZERO_FLAG, X == 0);
    MASK_ASSIGN(P, NEGATIVE_FLAG, (X & 0x80) != 0);
}

//
// Flags: CLC SEC CLI SEI CLD SED CLV
//

// CLC - Clear Carry
void NESCPU::CLC() {
    MASK_CLEAR(P, CARRY_FLAG);
}

// SEC - Set Carry
void NESCPU::SEC() {
    MASK_SET(P, CARRY_FLAG);
}

// CLI - Clear Interrupt Disable
void NESCPU::CLI() {
    MASK_CLEAR(P, INTERRUPT_DISABLE_FLAG);
}

// SEI - Set Interrupt Disable
void NESCPU::SEI() {
    MASK_SET(P, INTERRUPT_DISABLE_FLAG);
}

// SED - Set Decimal
void NESCPU::SED() {
    MASK_SET(P, DECIMAL_MODE_FLAG);
}

// CLD - Clear Decimal
void NESCPU::CLD() {
    MASK_CLEAR(P, DECIMAL_MODE_FLAG);
}

// CLV - Clear Overflow
void NESCPU::CLV() {
    MASK_CLEAR(P, OVERFLOW_FLAG);
}