#include "nesbus.hpp"
#include "nesemu.hpp"
#include "nescpu.hpp"
#include "nesppu.hpp"
#include "nesdsk.hpp"

NESBUS::NESBUS(NESEMU* emu) : NESComponent(emu) {
    ioregisters = (Uint8 *)SDL_malloc(0x20);            // 32 bytes for I/O registers (0x4000-0x401F)
    SDL_memset(ioregisters, 0, 0x20);                   // Clear I/O registers
}

NESBUS::~NESBUS() {
    if (ioregisters) {
        SDL_free(ioregisters);
        ioregisters = NULL;
    }
}

Uint8 NESBUS::read(Uint16 address) {
    if (address >= 0x0000 && address < 0x2000) {
        return emu->cpu->read(address); // Maps beautifully to WRAM
    } else if (address >= 0x2000 && address < 0x4000) {
        return emu->ppu->read(address); // Maps beautifully to PPU Registers
    } else if (address >= 0x4000 && address < 0x4020) {
        return ioregisters[address - 0x4000];
    } else if (address >= 0x4020) {
        return emu->dsk->read(address);  // Everything else falls to the Disk / Cartridge
    }
    return 0x00;
}

void NESBUS::write(Uint16 address, Uint8 value) {
    if (address >= 0x0000 && address < 0x2000) {
        emu->cpu->write(address, value);
    } else if (address >= 0x2000 && address < 0x4000) {
        emu->ppu->write(address, value);
    } else if (address >= 0x4000 && address < 0x4020) {
        ioregisters[address - 0x4000] = value;
    } else if (address >= 0x4020) {
        emu->dsk->write(address, value);
    }
}

Uint16 NESBUS::readWord(Uint16 address) {
    Uint8 low = read(address);
    Uint8 high = read(address + 1);
    return (high << 8) | low;
}

void NESBUS::writeWord(Uint16 address, Uint16 value) {
    write(address, value & 0xFF);
    write(address + 1, (value >> 8) & 0xFF);
}