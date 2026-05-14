#include "gbcpu.hpp"
#include "gbemu.hpp"
#include "gbbus.hpp"

GBCPU::GBCPU(GBEMU* emu) : GBComponent(emu) {}

void GBCPU::step(Uint64 deltaTime) {
    // Fetch opcode (1 byte)
    Uint16 pc = 0; // Program counter - for now hardcoded to 0
    Uint8 opcode = emu->bus->read8(pc);
    
    // For now, just increment PC and do nothing with opcode
    // In a real implementation, we would decode and execute the opcode
    // based on the Game Boy's LR35902 (Z80-like) instruction set
    (void)opcode; // Suppress unused variable warning
    (void)deltaTime; // Suppress unused parameter warning
}