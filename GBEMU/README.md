# GBEMU

GameBoy emulator based on SDL3 and C++. Target is Classic Game Boy (DMG), but could be extended to Pocket (MGB), Super Game Boy (SGB) and Game Boy Color (CGB).

## Components

* **SDLEMU**: bridge between SDL and GBEMU.
* **GBEMU**: emulator interface to SDLEMU and GB sub-components, running at Master Clock of ~4.19 MHz (4.194304 MHz).
* **GBCPU**: Core Processing Unit emulation based on a Sharp SM83 at a speed of ~1.05 MHz (1.048576 MHz) = clock / 4.
* **GBPPU**: Picture Processing Unit based on Sharp LR35902 at a speed of ~4.19 MHz (4.194304 MHz).
* **GBBUS**: Bus emulation for all memory read and writes (See memory mapping).
* **GBDSK**: cartridge emulation for ROMONLY, MBC1 etc ...

## Memory Mapping

GBBUS handles I/O registers and routes to all sub-components (DSK, CPU, PPU, ROM/RAM) based on address mapping:
- GBDSK holds Game ROM (gamerom) with variable length. It maintains roms pointers to 16 KB ROM banks and rams pointers to 8 KB RAM banks. romActive indexes the active ROM banks. ramActive indexes the active RAM banks.
- GBCPU holds 128 bytes High RAM (hram) and up to 8 * 4KB Work RAM (wram). wramActive indexes active RAM banks.
- GBPPU holds 160 bytes (40 * 4) OAM RAM (oam) and up to 2 * 8KB Video RAM (vram). GBPPU also holds RGBA format frameBuffers for SDL3 rendering. frameBufferActive indexes the active frameBuffer that SDL renders, while PPU writes to the other one.

## Building

### Prerequisites
- SDL3 development libraries
- C++ compiler with C++17 support
- Make or CMake

### Build Instructions
```bash
# Using Make (if Makefile exists)
make

# Or using CMake
mkdir build && cd build
cmake ..
make
```

## Running

```bash
./gbemu <path_to_gameboy_rom>
```

## Features

- Accurate CPU emulation (SM83 core)
- PPU with sprite and background rendering
- Memory Bank Controller (MBC) support:
  - ROM ONLY
  - MBC1
  - MBC1+RAM
  - MBC1+RAM+BATTERY
  - MBC2
  - MBC2+BATTERY
  - ROM+RAM
  - ROM+RAM+BATTERY
  - And more...
- Save state support (in development)
- Debugging capabilities

## Performance Target

The emulator aims to run at the original Game Boy clock speed of ~4.19 MHz for accurate timing and performance.

## References

- [Game Boy CPU Manual](https://archive.org/download/GameBoyManual/GameBoy.pdf)
- [Pan Docs](https://gbdev.io/pandocs/)
- [GBEDG (Game Boy Exact Documentation Guide)](https://github.com/GBEDG/gbcdocs)
