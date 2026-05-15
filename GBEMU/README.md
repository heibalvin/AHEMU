# GBEMU

GameBoy emulator based on SDL3 and C++. Target is Classic Game Boy (DMG), but could be extended to Pocket (MGB), Super Game Boy (SGB) and Game Boy Color (CGB).

## Components
 * SDLEMU: bridge between SDL and GBEMU.
 * GBEMU: emulator interface to SDLEMU and GB sub-components, running at Master Clock of ~4.19 MHz (4.194304 MHz).
 * GBCPU: Core Processng Unit emulation based on a Sharp SM83 at a speed of ~1.05 MHz (1.048576 MHz) = clock / 4.
 * GBPPU: Picture Processing Unit based on Sharp LR35902 at a speed of ~4.19 MHz (4.194304 MHz).
 * GBBUS: Bus emulation for all memory read and writes (See memory mapping).
 * GBDSK: cartridge emulation for ROMONLY, MBC1 etc ...

## Memory Mapping

NESBUS holds 128 bytes IO/Registers (ioregisters) and route to all below DSK, CPU, PPU roms/rams based on address.   
NESDSK holds Game ROM (gamerom) with variable length. It will keep roms pointers to 16 KB ROM bank and rams pointers to 8 KB RAM banks. romActive will index the active ROM banks. ramActive will index the active RAM banks.   
NESCPU holds 128 bytes High RAM (hram) and max 8 * 4KB Work RAM (wram). wramActive will index active RAM banks.   
NESPPU holds 160 bytes (40 * 4) OAM RAM (oam) and max 2 * 8KB Video RAM (vram). NESPPU will also hold RGBA format frameBuffers for SDL3 to render. frameBufferActive will index the active frameBuffer that SDL could render, whereas PPU will be writing on the other one.
