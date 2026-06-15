#include "GBDSK.h"
#include "GBEMU.h"

GBDSK::GBDSK(GBEMU *emu) 
    : GBCOM(emu), romData(nullptr), romSize(0), 
      romBank(1), ramBank(0), ramEnabled(false), bankingMode(false) {
    SDL_memset(externalRAM, 0, sizeof(externalRAM));
}

GBDSK::~GBDSK() {
    if (romData) {
        SDL_free(romData);
        romData = nullptr;
    }
}

void GBDSK::reset() { 
    romBank = 1; 
    ramBank = 0;
    ramEnabled = false; 
    bankingMode = false;
    SDL_memset(externalRAM, 0, sizeof(externalRAM));
}

Uint8 GBDSK::read(Uint16 addr) {
    // Bank 0 (Fixed): Always first 16KB
    if (addr < 0x4000) {
        return romData[addr];
    }
    // Switchable Bank (16KB)
    size_t offset = (size_t)romBank * 0x4000;
    return romData[offset + (addr & 0x3FFF)];
}

void GBDSK::write(Uint16 addr, Uint8 value) {
    // MBC1 Register Logic
    if (addr < 0x2000) {
        ramEnabled = ((value & 0x0F) == 0x0A);
    } 
    else if (addr >= 0x2000 && addr < 0x4000) {
        romBank = (romBank & 0x60) | (value & 0x1F);
        if ((romBank & 0x1F) == 0) romBank |= 1;
    }
    else if (addr >= 0x4000 && addr < 0x6000) {
        bankingMode = (value & 0x01);
        if (bankingMode) {
            ramBank = (value >> 4) & 0x03;
        } else {
            romBank = (romBank & 0x1F) | ((value & 0x03) << 5);
        }
    }
}

Uint8 GBDSK::readRam(Uint16 addr) {
    if (ramEnabled && addr >= 0xA000 && addr <= 0xBFFF) {
        return externalRAM[((size_t)ramBank * 0x2000) + (addr - 0xA000)];
    }
    return 0xFF;
}

void GBDSK::writeRam(Uint16 addr, Uint8 value) {
    if (ramEnabled && addr >= 0xA000 && addr <= 0xBFFF) {
        externalRAM[((size_t)ramBank * 0x2000) + (addr - 0xA000)] = value;
    }
}

void GBDSK::load(const Uint8* data, size_t size) {
    romSize = size;
    romData = (Uint8 *)SDL_malloc(romSize);
    SDL_memcpy(romData, data, romSize);
}