#include "gbbus.hpp"
#include "gbemu.hpp"
#include "gbcpu.hpp"
#include "gbppu.hpp"
#include "gbdsk.hpp"

GBBUS::GBBUS(GBEMU* emu) : GBComponent(emu) {
    // Initialize I/O Registers
    ioregisters = (Uint8 *)SDL_malloc(0x80); // 128 bytes for I/O registers
    SDL_memset(ioregisters, 0, 0x80); // Clear I/O registers
}

GBBUS::~GBBUS() {
    if (ioregisters) {
        SDL_free(ioregisters);
        ioregisters = NULL;
    }
}

Uint8 GBBUS::read(Uint16 address) {
    if (address < 0x4000) {
        return emu->dsk->roms[emu->dsk->romActive[0]][address];
    } else if (address >= 0x4000 && address < 0x8000) {
        return emu->dsk->roms[emu->dsk->romActive[1]][address - 0x4000];
    } else if (address >= 0x8000 && address < 0xA000) {
        return emu->ppu->vrams[emu->ppu->vramActive][address - 0x8000];
    } else if (address >= 0xA000 && address < 0xC000) {    
        return emu->dsk->rams[emu->dsk->ramActive][address - 0xA000];
    } else if (address >= 0xC000 && address < 0xD000) {
        return emu->cpu->wrams[emu->cpu->wramActive[0]][address - 0xC000];
    } else if (address >= 0xD000 && address < 0xE000) {
        return emu->cpu->wrams[emu->cpu->wramActive[1]][address - 0xE000];
    } else if (address >= 0xE000 && address < 0xF000) {
        return emu->cpu->wrams[emu->cpu->wramActive[0]][address - 0xE000];
    } else if (address >= 0xF000 && address < 0xFE00) {
        return emu->cpu->wrams[emu->cpu->wramActive[1]][address - 0xF000];
    } else if (address >= 0xFE00 && address < 0xFEA0) {
        return emu->ppu->oam[address - 0xFE00];
    } else if (address >= 0xFF00 && address < 0xFF80) {
        return ioregisters[address - 0xFF00];
    } else if (address >= 0xFF80 && address < 0xFFFF) {
        return emu->cpu->hram[address - 0xFF80];
    }

    // Default return for unimplemented areas
    return 0x00;
}

void GBBUS::write(Uint16 address, Uint8 value) {
    if (address < 0x4000) {
        // ROM area is typically read-only, ignore writes
    } else if (address >= 0x4000 && address < 0x8000) {
        // ROM area is typically read-only, ignore writes
    } else if (address >= 0x8000 && address < 0xA000) {
        emu->ppu->vrams[emu->ppu->vramActive][address - 0x8000] = value;
    } else if (address >= 0xA000 && address < 0xC000) {    
        emu->dsk->rams[emu->dsk->ramActive][address - 0xA000] = value;
    } else if (address >= 0xC000 && address < 0xD000) {
        emu->cpu->wrams[emu->cpu->wramActive[0]][address - 0xC000] = value;
    } else if (address >= 0xD000 && address < 0xE000) {
        emu->cpu->wrams[emu->cpu->wramActive[1]][address - 0xD000] = value;
    } else if (address >= 0xE000 && address < 0xF000) {
        emu->cpu->wrams[emu->cpu->wramActive[0]][address - 0xE000] = value;
    } else if (address >= 0xF000 && address < 0xFE00) {
        emu->cpu->wrams[emu->cpu->wramActive[1]][address - 0xF000] = value;
    } else if (address >= 0xFE00 && address < 0xFEA0) {
        emu->ppu->oam[address - 0xFE00] = value;
    } else if (address >= 0xFF00 && address < 0xFF80) {
        ioregisters[address - 0xFF00] = value;
    } else if (address >= 0xFF80 && address < 0xFFFF) {
        emu->cpu->hram[address - 0xFF80] = value;
    }
}