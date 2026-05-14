#include "gbbus.hpp"
#include "gbemu.hpp"
#include "gbcpu.hpp"
#include "gbppu.hpp"
#include "gbdsk.hpp"

GBBUS::GBBUS(GBEMU* emu) : GBComponent(emu) {}

Uint8 GBBUS::read8(Uint16 address) {
    // Game Boy Memory Map:
    // 0000-3FFF: ROM Bank 0 (16KB)
    // 4000-7FFF: ROM Bank 1 (16KB, switchable)
    // 8000-9FFF: Video RAM (VRAM, 8KB)
    // A000-BFFF: External RAM (8KB, switchable)
    // C000-CFFF: Work RAM (WRAM, 4KB)
    // D000-DFFF: Work RAM (WRAM, 4KB, switchable in CGB)
    // E000-FDFF: Echo RAM (mirror of C000-DDFF)
    // FE00-FE9F: OAM (Object Attribute Memory)
    // FEA0-FEFF: Not Usable
    // FF00-FF7F: I/O Registers
    // FF80-FFFE: High RAM (HRAM)
    // FFFF: Interrupt Enable Register (IE)
    
    // For now, we'll focus on ROM access and basic I/O
    if (address >= 0x0000 && address <= 0x7FFF) {
        // ROM Banks 0 and 1
        return emu->dsk->readRom(address);
    }
    else if (address >= 0x8000 && address <= 0x9FFF) {
        // VRAM - not implemented yet
        return 0x00;
    }
    else if (address >= 0xA000 && address <= 0xBFFF) {
        // External RAM - not implemented yet
        return 0x00;
    }
    else if (address >= 0xC000 && address <= 0xDFFF) {
        // Work RAM - not implemented yet
        return 0x00;
    }
    else if (address >= 0xE000 && address <= 0xFDFF) {
        // Echo RAM - mirror of C000-DDFF
        Uint16 mirroredAddress = address - 0x2000;
        return read8(mirroredAddress);
    }
    else if (address >= 0xFE00 && address <= 0xFE9F) {
        // OAM - not implemented yet
        return 0x00;
    }
    else if (address >= 0xFEA0 && address <= 0xFEFF) {
        // Not Usable - returns 0xFF on reads
        return 0xFF;
    }
    else if (address >= 0xFF00 && address <= 0xFF7F) {
        // I/O Registers
        // For now, handle a few basic ones
        if (address == 0xFF40) { // LCDC - LCD Control
            return 0x91; // Default value
        }
        if (address == 0xFF41) { // STAT - LCD Status
            return 0x80; // Default value
        }
        if (address == 0xFF42) { // SCY - Scroll Y
            return 0x00;
        }
        if (address == 0xFF43) { // SCX - Scroll X
            return 0x00;
        }
        if (address == 0xFF44) { // LY - LCDC Y-Coordinate
            return 0x90; // Value after VBlank
        }
        if (address == 0xFF45) { // LYC - LY Compare
            return 0x00;
        }
        if (address == 0xFF46) { // DMA - DMA Transfer and Start
            return 0xFF;
        }
        if (address == 0xFF47) { // BGP - BG Palette Data
            return 0xFC;
        }
        if (address == 0xFF48) { // OBP0 - Object Palette 0 Data
            return 0xFF;
        }
        if (address == 0xFF49) { // OBP1 - Object Palette 1 Data
            return 0xFF;
        }
        if (address == 0xFF4A) { // WY - Window Y
            return 0x00;
        }
        if (address == 0xFF4B) { // WX - Window X minus 7
            return 0x00;
        }
        if (address == 0xFFFF) { // IE - Interrupt Enable
            return 0x00;
        }
        return 0x00; // Default for unimplemented I/O
    }
    else if (address >= 0xFF80 && address <= 0xFFFE) {
        // HRAM - not implemented yet
        return 0x00;
    }
    
    // Default return for unimplemented areas
    return 0x00;
}

void GBBUS::write8(Uint16 address, Uint8 value) {
    // Game Boy Memory Map:
    // 0000-3FFF: ROM Bank 0 (16KB) - Writes go to MBC if present
    // 4000-7FFF: ROM Bank 1 (16KB, switchable) - Writes go to MBC if present
    // 8000-9FFF: Video RAM (VRAM, 8KB)
    // A000-BFFF: External RAM (8KB, switchable)
    // C000-CFFF: Work RAM (WRAM, 4KB)
    // D000-DFFF: Work RAM (WRAM, 4KB, switchable in CGB)
    // E000-FDFF: Echo RAM (mirror of C000-DDFF)
    // FE00-FE9F: OAM (Object Attribute Memory)
    // FEA0-FEFF: Not Usable
    // FF00-FF7F: I/O Registers
    // FF80-FFFE: High RAM (HRAM)
    // FFFF: Interrupt Enable Register (IE)
    
    if (address >= 0x0000 && address <= 0x7FFF) {
        // ROM area - writes would go to MBC controller
        // For now, ignore writes to ROM (no MBC implemented)
    }
    else if (address >= 0x8000 && address <= 0x9FFF) {
        // VRAM - not implemented yet
    }
    else if (address >= 0xA000 && address <= 0xBFFF) {
        // External RAM - not implemented yet
    }
    else if (address >= 0xC000 && address <= 0xDFFF) {
        // Work RAM - not implemented yet
    }
    else if (address >= 0xE000 && address <= 0xFDFF) {
        // Echo RAM - mirror of C000-DDFF
        Uint16 mirroredAddress = address - 0x2000;
        write8(mirroredAddress, value);
    }
    else if (address >= 0xFE00 && address <= 0xFE9F) {
        // OAM - not implemented yet
    }
    else if (address >= 0xFEA0 && address <= 0xFEFF) {
        // Not Usable - writes ignored
    }
    else if (address >= 0xFF00 && address <= 0xFF7F) {
        // I/O Registers
        if (address == 0xFF40) { // LCDC - LCD Control
            // Ignore for now
        }
        else if (address == 0xFF41) { // STAT - LCD Status
            // Only bits 0-2 are writable
            // Ignore for now
        }
        else if (address == 0xFF42) { // SCY - Scroll Y
            // Ignore for now
        }
        else if (address == 0xFF43) { // SCX - Scroll X
            // Ignore for now
        }
        else if (address == 0xFF44) { // LY - LCDC Y-Coordinate (read-only)
            // Writes ignored
        }
        else if (address == 0xFF45) { // LYC - LY Compare
            // Ignore for now
        }
        else if (address == 0xFF46) { // DMA - DMA Transfer and Start
            // Ignore for now
        }
        else if (address == 0xFF47) { // BGP - BG Palette Data
            // Ignore for now
        }
        else if (address == 0xFF48) { // OBP0 - Object Palette 0 Data
            // Ignore for now
        }
        else if (address == 0xFF49) { // OBP1 - Object Palette 1 Data
            // Ignore for now
        }
        else if (address == 0xFF4A) { // WY - Window Y
            // Ignore for now
        }
        else if (address == 0xFF4B) { // WX - Window X minus 7
            // Ignore for now
        }
        else if (address == 0xFFFF) { // IE - Interrupt Enable
            // Ignore for now
        }
    }
    else if (address >= 0xFF80 && address <= 0xFFFE) {
        // HRAM - not implemented yet
    }
    // Writes to unimplemented areas are ignored
}