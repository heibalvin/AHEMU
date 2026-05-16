#include "nescpu.hpp"
#include "nesemu.hpp"
#include "nesbus.hpp"

NESCPU::NESCPU(NESEMU* emu) : NESComponent(emu) {
    // Initialize WRAM
    wram = (Uint8 *)SDL_malloc(0x0800); // 2KB of WRAM
    SDL_memset(wram, 0, 0x0800); // Clear WRAM
}

NESCPU::~NESCPU() {
    if (wram) {
        SDL_free(wram);
        wram = NULL;
    }
}

void NESCPU::powerOn() {
    // Reset CPU state, registers, and flags
}

void NESCPU::step() {
    fetch();
    decode();
    execute();
}

void NESCPU::fetch() {
    // TBD: Implement instruction fetching logic, which involves reading the next instruction from memory using the Program Counter (PC) and incrementing the PC accordingly.
}

void NESCPU::decode() {
    // TBD: Implement instruction decoding logic, which involves interpreting the fetched instruction and determining which operation to perform, as well as which registers or memory locations are involved.
}

void NESCPU::execute() {
    // TBD: Implement instruction execution logic, which involves performing the operation specified by the decoded instruction, updating registers, memory, and flags as necessary.
}

