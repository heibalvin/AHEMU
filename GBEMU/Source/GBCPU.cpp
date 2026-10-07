#include "GBCPU.h"
#include "GBEMU.h" // Needed to access the bus for fetching

GBCPU::GBCPU(GBEMU &emu) : GBCOM(emu) {
    reset();
}

void GBCPU::powerOn() { reset(); }
void GBCPU::powerOff() {}

void GBCPU::reset() {
	SDL_memset(HRAM, 0, sizeof(HRAM));

    regs.AF = 0x01B0; // Default power-on state
    regs.BC = 0x0013;
    regs.DE = 0x00D8;
    regs.HL = 0x014D;
    regs.SP = 0xFFFE;
    regs.PC = 0x0100;
}

Uint8 GBCPU::read(Uint16 addr) {
    return HRAM[addr & 0x7F];
}

void GBCPU::write(Uint16 addr, Uint8 value) {
    HRAM[addr & 0x7F] = value;
}

int GBCPU::step() {
    if (emu.IME && emu.isIRQRequested) {
        handleInterrupts();
    }

    fetch(); 

    if (opcode == nullptr || opcode->execute == nullptr) {
        SDL_Log("ERROR: Missing/Invalid Opcode 0x%02X at PC 0x%04X", bytes[0], regs.PC);
        emu.isRunning = false;
        return 0;
    }

    decode(); 
    execute();
    
    regs.PC = nextPC;
    int cycles = opcode->cycles; // Store cycles before clearing state
    
    return cycles; // Return the stored value
}

void GBCPU::handleInterrupts() {
    if (emu.IME && emu.isIRQRequested) {
        // 1. Disable IME
        emu.IME = false;

        // 2. Push PC to stack
        // 3. Jump to Interrupt Vector
    }
}

void GBCPU::fetch() {
    bytes[0] = emu.bus.read(regs.PC);
    opcode = nullptr;
    opcode = &opcodes[bytes[0]];

    // Pre-load operands based on instruction length
    // If length is 2, load 1 byte; if 3, load 2 bytes
    if (opcode->length >= 2) bytes[0] = emu.bus.read(regs.PC + 1);
    if (opcode->length == 3) bytes[1] = emu.bus.read(regs.PC + 2);
}

void GBCPU::decode() {
    // Log format: [Registers] | [PC: Byte0 Byte1 Byte2] | [Mnemonic]
    if opcode->length == 1 {
        SDL_Log("AF:%04X BC:%04X DE:%04X HL:%04X SP:%04X | PC:%04X: %02X .. .. | %s",
            regs.AF, regs.BC, regs.DE, regs.HL, regs.SP,
            regs.PC, bytes[0], bytes[1], bytes[2],
            opcode->mnemonic);
    }
    
}

void GBCPU::execute() {
    nextPC = regs.PC + opcode->length;
    (this->*opcode->execute)(); // Execute the logic
}

void GBCPU::NOP() {
    // do nothing
}

void GBCPU::LD_BC_d16() {
    // bytes[0] is the low byte, bytes[1] is the high byte
    regs.BC = (bytes[2] << 8) | bytes[1];
}

void GBCPU::PUSH_BC() {
    regs.SP -= 2;
    emu.bus.write(regs.SP, regs.BC & 0xFF);     // Low byte
    emu.bus.write(regs.SP + 1, (regs.BC >> 8)); // High byte
}

void GBCPU::POP_BC() {
    regs.BC = emu.bus.read(regs.SP) | (emu.bus.read(regs.SP + 1) << 8);
    regs.SP += 2;
}

void GBCPU::CALL_nn() {
    Uint16 target = (bytes[1] << 8) | bytes[0];
    regs.SP -= 2;
    emu.bus.write(regs.SP, (nextPC & 0xFF));     // Push current nextPC (return address)
    emu.bus.write(regs.SP + 1, (nextPC >> 8));
    nextPC = target; // Jump
}

void GBCPU::RET() {
    Uint16 addr = emu.bus.read(regs.SP) | (emu.bus.read(regs.SP + 1) << 8);
    regs.SP += 2;
    nextPC = addr;
}

void GBCPU::EI() { 
    // Set internal Master Interrupt Enable (IME) flag to true
    emu.IME = true; 
}

void GBCPU::DI() { 
    // Set internal Master Interrupt Enable (IME) flag to false
    emu.IME = false; 
}