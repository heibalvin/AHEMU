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

    P = P | INTERRUPT_DISABLE_FLAG; // Clear Processor Status
    PC = 0x0000; // Reset Program Counter to 0x0000
    SP = 0xFF; // Initialize Stack Pointer to 0xFF (empty stack)
    
    // Clear WRAM
    SDL_memset(wram, 0, 0x0800);

    BRK(); // Execute BRK to start execution at the reset vector
    PC = nextPC;

    SDL_Log("NESCPU: PC start address %04X", PC);
}

void NESCPU::opcodesInit() {
    // TBD: Initialize the opcodes with the appropriate mnemonics, operation function pointers, and cycle counts for each of the 256 opcodes.
    for(int i = 0; i < 256; i++) {
        opcodes[i] = {"UNKNOWN", NULL, IMPLIED, 1, 1};      // Default to an invalid opcode
    }

    // Access LDA STA LDX STX LDY STY
    opcodes[0xA2] = {"LDX #%02X", &NESCPU::LDX, IMMEDIATE, 2, 2};
    opcodes[0xA6] = {"LDX %02X", &NESCPU::LDX, ZERO_PAGE, 2, 3};
    opcodes[0xB6] = {"LDX %02X, Y", &NESCPU::LDX, ZERO_PAGE_Y, 2, 4};
    opcodes[0xAE] = {"LDX %04X", &NESCPU::LDX, ABSOLUTE, 3, 4};
    opcodes[0xBE] = {"LDX %04X, Y", &NESCPU::LDX, ABSOLUTE_Y, 3, 4};

    // Jump: JMP JSR RTS BRK RTI
    opcodes[0x4C] = {"JMP %04X", &NESCPU::JMP, ABSOLUTE, 3, 3};
    opcodes[0x6C] = {"JMP (%04X)", &NESCPU::JMP, INDIRECT, 3, 5};
    opcodes[0x20] = {"JSR %04X", &NESCPU::JSR, ABSOLUTE, 3, 6};
    opcodes[0x60] = {"RTS", &NESCPU::RTS, IMPLIED, 1, 6};
    opcodes[0x00] = {"BRK", &NESCPU::BRK, IMMEDIATE, 2, 7};     // TBC: could be considered 1 (.IMPLIED) or 2 bytes (.IMMEDIATE) depending on how the instruction is implemented.
    opcodes[0x40] = {"RTI", &NESCPU::RTI, IMPLIED, 1, 6};

    // Stack: PHA PLA PHP PLP TXS TSX
    opcodes[0x48] = {"PHA", &NESCPU::PHA, IMPLIED, 1, 3};
    opcodes[0x68] = {"PLA", &NESCPU::PLA, IMPLIED, 1, 3};
    opcodes[0x08] = {"PHP", &NESCPU::PHP, IMPLIED, 1, 3};
    opcodes[0x28] = {"PLP", &NESCPU::PLP, IMPLIED, 1, 3};
    opcodes[0x9A] = {"TXS", &NESCPU::TXS, IMPLIED, 1, 2};
    opcodes[0xBA] = {"TSX", &NESCPU::TSX, IMPLIED, 1, 2};

    // Flags: CLC SEC CLI SEI CLD SED CLV
    opcodes[0x18] = {"CLC - Clear Carry", &NESCPU::CLC, IMPLIED, 1, 2};
    opcodes[0x38] = {"SEC - Set Carry", &NESCPU::SEC, IMPLIED, 1, 2};
    opcodes[0x58] = {"CLI - Clear Interrupt Disable", &NESCPU::CLI, IMPLIED, 1, 2};
    opcodes[0x78] = {"SEI - Set Interrupt Disable", &NESCPU::SEI, IMPLIED, 1, 2};
    opcodes[0xF8] = {"SED - Set Decimal", &NESCPU::SED, IMPLIED, 1, 2};
    opcodes[0xD8] = {"CLD - Clear Decimal", &NESCPU::CLD, IMPLIED, 1, 2};
    opcodes[0xB8] = {"CLV - Clear Overflow", &NESCPU::CLV, IMPLIED, 1, 2};

    // Other
    opcodes[0xEA] = {"NOP", NULL, IMPLIED, 1, 2};               

}

void NESCPU::step() {
    if (waitCycles > 0) {
        waitCycles--; // Decrement wait cycles if we're still waiting for the current instruction to complete
        return; // Skip fetching a new instruction until the current one is done
    }

    fetch();
    decode();
    execute();
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
        SDL_snprintf(mnemonic, sizeof(mnemonic), opcode->nmemonic, Uint16(bytes[1]) << 8 | Uint16(bytes[2]));
    }

    SDL_Log("%s | %s | %s", debugger, program, mnemonic); // Log the current CPU state, program bytes, and mnemonic for debugging
}

void NESCPU::execute() {
    // TBD: Implement instruction execution logic, which involves performing the operation specified by the decoded instruction, updating registers, memory, and flags as necessary.
    nextPC = PC + opcode->length;       // Increment the Program Counter to point to the next instruction 

    if (opcode->operation) {
        (this->*opcode->operation)();   // Call the operation function pointer for the current opcode
    }

    waitCycles = opcode->cycles - 1;    // Set the wait cycles for the current instruction (subtract 1 because we will execute the next instruction on the next step)
    PC = nextPC;                        // Update the Program Counter to the next instruction after execution
}

void NESCPU::NMIInterrupt() {
    // TBD: Implement Non-Maskable Interrupt (NMI) handling logic, which involves pushing the current PC and P onto the stack, setting the appropriate flags, and jumping to the NMI vector address.
}

void NESCPU::IRQInterrupt() {
    // TBD: Implement Interrupt Request (IRQ) handling logic, which involves checking the Interrupt Disable flag, and if interrupts are enabled, pushing the current PC and P onto the stack, setting the appropriate flags, and jumping to the IRQ vector address.
}

void NESCPU::checkZero(Uint8 value) {
    if (value == 0) {
        P = P | ZERO_FLAG;
    } else {
        P = P & ~ZERO_FLAG;
    }
}

void NESCPU::checkNegative(Uint8 value) {
    if ((value & 0x80) == 0x80) {
        P = P | NEGATIVE_FLAG;
    } else {
        P = P & ~NEGATIVE_FLAG;
    }
}

Uint16 NESCPU::getAddressMode() {
    Uint16 addr = Uint16(bytes[2]) << 8 | Uint16(bytes[1]);
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
            return addr;
            break;
        case ABSOLUTE_X:
            return addr + Uint16(X);
            break;
        case ABSOLUTE_Y:
            return addr + Uint16(Y);
            break;
        case INDIRECT:
            return emu->bus->readWord(addr);
            break;
        default:
            return 0x0000;
            break;
    }
}

//
// Access LDA STA LDX STX LDY STY
//
void NESCPU::LDX() {
    Uint16 addr = getAddressMode();
    X = emu->bus->read(addr);
    checkZero(X);
    checkNegative(X);
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
    wram[0x0100 + SP] = ((PC+2) >> 8) & 0xFF;                                   // Push high byte of PC+2
    wram[0x0100 + SP - 1] = (PC+2) & 0xFF;                                      // Push low byte of PC+2
    SP -= 2;
    nextPC = getAddressMode();
}

// RTS - Return from Subroutine
void NESCPU::RTS() {
    SP += 2;
    nextPC = Uint16(wram[0x0100 + SP]) << 8 | Uint16(wram[0x0100 + SP - 1]) + 1;
}

// BRK - Break (software IRQ)
void NESCPU::BRK() {
    wram[0x0100 + SP] = (PC >> 8) & 0xFF;                                   // Push high byte of PC
    wram[0x0100 + SP - 1] = PC & 0xFF;                                      // Push low byte of PC
    SP -= 2;
    PHP();                                                                  // Push Processor Status
    P = P | INTERRUPT_DISABLE_FLAG;                                         // Set Interrupt Disable flag
    nextPC = ((emu->bus->read(0xFFFF) << 8) | emu->bus->read(0xFFFE));      // Jump to BRK vector address
}

// RTI - Return from Interrupt
void NESCPU::RTI() {
    PLP();                                                                   // Push Processor Status
    SP += 2;
    nextPC = Uint16(wram[0x0100 + SP]) << 8 | Uint16(wram[0x0100 + SP - 1]);
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
}

// PHP - Push Processor Status
void NESCPU::PHP() {
    wram[0x0100 + SP] = ((P & ~UNUSED_FLAG) & ~BREAK_COMMAND_FLAG);
    SP -= 1;
}

// PLP - Pull Processor Status
void NESCPU::PLP() {
    SP += 1;
    P = wram[0x0100 + SP] | NEGATIVE_FLAG | OVERFLOW_FLAG | DECIMAL_MODE_FLAG | INTERRUPT_DISABLE_FLAG | ZERO_FLAG | CARRY_FLAG;
}

// TXS - Transfer X to Stack Pointer
void NESCPU::TXS() {
    SP = X;
}

// TSX - Transfer Stack Pointer to X
void NESCPU::TSX() {
    X = SP;
    checkZero(X);
    checkNegative(X);
}

//
// Flags: CLC SEC CLI SEI CLD SED CLV
//

// CLC - Clear Carry
void NESCPU::CLC() {
    P = P & ~CARRY_FLAG;
}

// SEC - Set Carry
void NESCPU::SEC() {
    P = P | CARRY_FLAG;
}

// CLI - Clear Interrupt Disable
void NESCPU::CLI() {
    P = P & ~INTERRUPT_DISABLE_FLAG;
}

// SEI - Set Interrupt Disable
void NESCPU::SEI() {
    P = P | INTERRUPT_DISABLE_FLAG;
}

// SED - Set Decimal
void NESCPU::SED() {
    P = P | DECIMAL_MODE_FLAG;
}

// CLD - Clear Decimal
void NESCPU::CLD() {
    P = P & ~DECIMAL_MODE_FLAG;
}

// CLV - Clear Overflow
void NESCPU::CLV() {
    P = P & ~OVERFLOW_FLAG;
}