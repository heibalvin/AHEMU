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
        // WRAM (mirrored every 2KB)
        return emu->cpu->wram[address % 0x0800];
    } else if (address >= 0x2000 && address < 0x4000) {
        // Central dispatcher hands off to the PPU component.
        // It mirrors every 8 bytes ($2000-$2007)
        return emu->ppu->read(address);
    } else if (address >= 0x4000 && address < 0x4020) {
        // APU and I/O Registers
        // TBD: Implement APU and I/O register reads
        return ioregisters[address - 0x4000];
    } else if (address >= 0x6000 && address < 0x8000) {
        // Cartridge space (PRG RAM)
        return emu->dsk->prgRams[0][address - 0x6000]; // Return from first PRG RAM bank if present
    } else if (address >= 0x8000 && address < 0xC000) {
        // Cartridge space (PRG ROM)
        return emu->dsk->prgRoms[emu->dsk->prgRomActive[0]][address - 0x8000];
    } else if (address >= 0xC000) {
        // Cartridge space (PRG ROM)
        return emu->dsk->prgRoms[emu->dsk->prgRomActive[1]][address - 0xC000]; // Return from last PRG ROM bank
    }

    // Default return for unimplemented areas
    return 0x00;
}

void NESBUS::write(Uint16 address, Uint8 value) {
    if (address >= 0x0000 && address < 0x2000) {
        // WRAM (mirrored every 2KB)
        emu->cpu->wram[address % 0x0800] = value;
    } else if (address >= 0x2000 && address < 0x4000) {
        // Central dispatcher passes register writes straight to the PPU
        emu->ppu->write(address, value);
    } else if (address >= 0x4000 && address < 0x4020) {
        // APU and I/O Registers
        // TBD: Implement APU and I/O register reads
        ioregisters[address - 0x4000] = value;
    } else if (address >= 0x6000 && address < 0x8000) {
        // Cartridge space (PRG RAM)
        emu->dsk->prgRams[0][address - 0x6000] = value; // Write to first PRG RAM bank if present
    } else if (address >= 0x8000 && address < 0xC000) {
        // Cartridge space (PRG ROM)
        // Writes to PRG ROM area are typically ignored or used for mapper control
        emu->dsk->prgRoms[emu->dsk->prgRomActive[0]][address - 0x8000] = value; // For now, we can allow writes for mapper control, but this may need to be handled differently based on the mapper used
    }   else if (address >= 0xC000) {
        // Writes to other areas are typically ignored
        emu->dsk->prgRoms[emu->dsk->prgRomActive[1]][address - 0xC000] = value; // For now, we can allow writes for mapper control, but this may need to be handled differently based on the mapper used
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