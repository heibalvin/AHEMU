#include "gbcpu.hpp"
#include "gbemu.hpp"
#include "gbbus.hpp"

GBCPU::GBCPU(GBEMU* emu) : GBComponent(emu) {
    
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

GBCPU::~GBCPU() {
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

void GBCPU::step() {
    // TBD: Implement CPU execution logic, including fetching instructions from memory, decoding them, and executing them. This will involve interacting with the bus to read/write memory and registers, as well as updating the CPU state accordingly.
}