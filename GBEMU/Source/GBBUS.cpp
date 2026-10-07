#include "GBBUS.h"
#include "GBEMU.h" // ADD THIS LINE: It provides the full definition of GBEMU

GBBUS::GBBUS(GBEMU &emu) 
    : GBCOM(emu) {
	reset();
}

GBBUS::~GBBUS() {
}

void GBBUS::reset() {
	SDL_memset(WRAM, 0, sizeof(WRAM));
}

Uint8 GBBUS::read(Uint16 addr) {
    // 0x0000 - 0x7FFF: Cartridge ROM
    if (addr <= 0x7FFF) {
        return emu.dsk.read(addr);
    }

    // 0x8000 - 0x9FFF: VRAM (Blocked during PPU Mode 3)
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        if (emu.vdp.getMode() == 3) return 0xFF;
        return emu.vdp.read(addr);
    }

    // 0xA000 - 0xBFFF: Cartridge RAM (Banked)
    if (addr >= 0xA000 && addr <= 0xBFFF) {    
        return emu.dsk.readRam(addr);
    }

    // 0xC000 - 0xDFFF: WRAM
    if (addr >= 0xC000 && addr <= 0xDFFF) {
        return WRAM[addr & 0x1FFF];
    }

    // 0xE000 - 0xFDFF: Echo RAM (Mirror of WRAM)
    if (addr >= 0xE000 && addr <= 0xFDFF) {
        return WRAM[addr & 0x1FFF];
    }

    // 0xFE00 - 0xFE9F: OAM (Blocked during PPU Mode 2/3 OR DMA)
    if (addr >= 0xFE00 && addr <= 0xFE9F) {
        if (emu.vdp.isDmaActive() || emu.vdp.getMode() >= 2) return 0xFF;
        return emu.vdp.read(addr);
    }

    // 0xFF00 - 0xFF7F: I/O Registers
    if (addr >= 0xFF00 && addr <= 0xFF7F) {
        if (addr == 0xFF00) return emu.joy.read(addr);
        if (addr == 0xFF0F) return emu.IE;          // Interrupts
        if (addr >= 0xFF40 && addr <= 0xFF4B) {
            return emu.vdp.read(addr);         // VDP Registers like LCDC, STAT, etc.
        }
        if (addr >= 0xFF10 && addr <= 0xFF3F) {
            return emu.apu.read(addr);         // APU Registers ...
        }
    }

    // 0xFF80 - 0xFFFF: HRAM
    if (addr >= 0xFF80 && addr <= 0xFFFF) {
        if (addr == 0xFFFF) {
            return emu.IE;
        }
        return emu.cpu.read(addr);
    }

    return 0xFF; // Default for open bus
}

void GBBUS::write(Uint16 addr, Uint8 value) {
    // 0x0000 - 0x7FFF: Cartridge (Bank Switching)
    if (addr <= 0x7FFF) { 
        emu.dsk.write(addr, value);
    }

    // 0x8000 - 0x9FFF: VRAM (Blocked during PPU Mode 3)
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        if (emu.vdp.getMode() == 3) return;
        emu.vdp.write(addr, value);
    }

    // 0xA000 - 0xBFFF: Cartridge RAM
    if (addr >= 0xA000 && addr <= 0xBFFF) { 
        emu.dsk.writeRam(addr, value);
    }

    // 0xC000 - 0xDFFF: WRAM
    if (addr >= 0xC000 && addr <= 0xDFFF) { 
        WRAM[addr & 0x1FFF] = value;
    }

    // 0xFE00 - 0xFE9F: OAM (Blocked during PPU Mode 2/3 OR DMA)
    if (addr >= 0xFE00 && addr <= 0xFE9F) {
        if (emu.vdp.isDmaActive() || emu.vdp.getMode() >= 2) return;
        emu.vdp.write(addr, value); return;
    }

    // 0xFF00 - 0xFF7F: I/O Registers
    if (addr >= 0xFF00 && addr <= 0xFF7F) {
        if (addr == 0xFF00) emu.joy.write(addr, value);
        if (addr == 0xFF0F) {
            emu.IF = value;
            emu.updateIRQRequest();
        }
        if (addr >= 0xFF40 && addr <= 0xFF4B) {
            emu.vdp.write(addr, value);         // VDP Registers like LCDC, STAT, etc.
        }
        if (addr >= 0xFF10 && addr <= 0xFF3F) {
            emu.apu.write(addr, value);         // APU Registers ...
        }
    }

    // 0xFF80 - 0xFFFF: HRAM
    if (addr >= 0xFF80 && addr <= 0xFFFF) { 
        emu.cpu.write(addr, value);

        if (addr == 0xFFFF) {
            emu.IE = value;
            emu.updateIRQRequest();
        }
        return;   
    }


}

// 16-bit Little Endian Helpers
Uint16 GBBUS::readLE(Uint16 addr) {
    return (Uint16)(read(addr) | (read(addr + 1) << 8));
}

void GBBUS::writeLE(Uint16 addr, Uint16 value) {
    write(addr, (Uint8)(value & 0xFF));
    write(addr + 1, (Uint8)((value >> 8) & 0xFF));
}

// 16-bit Big Endian Helpers
Uint16 GBBUS::readBE(Uint16 addr) {
    return (Uint16)((read(addr) << 8) | read(addr + 1));
}

void GBBUS::writeBE(Uint16 addr, Uint16 value) {
    write(addr, (Uint8)((value >> 8) & 0xFF));
    write(addr + 1, (Uint8)(value & 0xFF));
}