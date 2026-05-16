#include "nescpu.hpp"
#include "nesemu.hpp"
#include "nesbus.hpp"

NESCPU::NESCPU(NESEMU* emu) : NESComponent(emu) {
    // Initialize HRAM
    hram = (Uint8 *)SDL_malloc(0x80); // 128 bytes of HRAM
    SDL_memset(hram, 0, 0x80); // Clear HRAM
    
    // Initialize WRAM
    wramCount = 1;
    for (int i = 0; i < wramCount; i++) {
        wrams[i] = (Uint8 *)SDL_malloc(0x1000); // 4KB of WRAM
        SDL_memset(wrams[i], 0, 0x1000); // Clear WRAM
    }
    wramActive[0] = 0;
    wramActive[1] = wramCount - 1;
}

NESCPU::~NESCPU() {
    if (hram) {
        SDL_free(hram);
        hram = NULL;
    }

    for (int i = 0; i < wramCount; i++) {
        if (wrams[i]) {
            SDL_free(wrams[i]);
            wrams[i] = NULL;
        }
    }
}

void NESCPU::powerOn() {
    // Reset registers to their default power-on state
    AF = 0x0000;
    BC = 0x0000;
    DE = 0x0000;
    HL = 0x0000;
    SP = 0xFFFE; // Stack Pointer starts at the end of WRAM
    PC = 0x0100; // Program Counter starts at the entry point of the ROM

    // Clear HRAM and WRAM
    SDL_memset(hram, 0, 0x80);
    for (int i = 0; i < wramCount; i++) {
        SDL_memset(wrams[i], 0, 0x1000);
    }
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

