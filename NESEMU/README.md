# SDL3 NES Emulator (NESEMU)

## Table of Contents
- [Repo Structure](#repository)
- [Install](#install)
- [Components](#components)

## Repository

representation and description of Build, Output, Resources, Source and Makefile.

## Install

Instructions to install SDL3 `brew install sdl3`. specific version stable 3.4.8.

To build the emulator, run `make` in the NESEMU directory.
To run the emulator, run `make run`.
This will compile and execute the SDL3 NESPPU demo.

## Components

Components:
 * [SDLEMU](#sdlemu)
 * [NESEMU](#nesemu)
 * [NESBUS](#nesbus)
 * [NESDSK](#nesdsk)
 * [NESCPU](#nescpu)
 * [NESPPU](#nesppu)
 * [NESAPU](#nesapu)

### SDLEMU

#### Description

SDL3 Interface between main.cpp and NESEMU. It will handle all macOS / iOS platform related compoennt like File, Window, Video, Sound, Input, Timer, etc ...

#### Status
 
Implemented for File, Window, Video, Timer.
Implemented SDL component management
 * initSDL()
 * releaseSDL()

Implemented File management
 * getDirectoryPath()
 * getFilePath()
 * loadPNG()
 * loadFile()
    
Implemented ROM management
 * loadRom()
 * exportCHR2RGBA()
    
Implemented Application LifeCycle - for debugging step by step
 * input()
 * update()
 * render()

#### Next Step

Input and Sound.


### NESCPU

### Description

Emulates the MOS 6502 central processing unit. It handles instruction decoding, execution, and cycles.

### Status

Functionalities:
 * Work RAM (WRAM) and Registers (A, X, Y, P, SP & PC).
 * powerOn()
 * reset()
 * step() - with waitCycle functionality to micmic multi-cycle instructions
 * fetch()
 * decode()
 * execute()

Opcodes: [NESDEV Instruction Reference](https://www.nesdev.org/wiki/Instruction_reference#CLD)
 * Jump: JMP JSR RTS BRK RTI
 * Stack: PHA PLA PHP PLP TXS TSX
 * Flags: CLC SEC CLI SEI CLD SED CLV
 * Others: NOP

### Next Step

Functionality:
 * NMIInterrupt()
 * IRQInterrupt()

Opcodes:
 * Access LDA STA LDX STX LDY STY		
 * Transfer TAX TXA TAY TYA				
 * Arithmetic ADC SBC INC DEC INX DEX INY DEY
 * Shift ASL LSR ROL ROR				
 * Bitwise AND ORA EOR BIT				
 * Compare CMP CPX CPY					
 * Branch BCC BCS BEQ BNE BPL BMI BVC BVS

### NESEMU
 * **Description:** Main emulator core that coordinates CPU and PPU steps, synchronizing timing and handling interrupts.
 * **Status:** Implemented
 * **Next Steps:** Implement interrupt request (IRQ) and non-maskable interrupt (NMI) handling, and add frame timing control for stable FPS.

### NESBUS
**Description:** Handles the system bus, routing memory accesses to the appropriate subsystems (CPU, PPU, APU, and cartridge).
**Status:** Implemented
**Next Steps:** Ensure proper handling of open bus behavior and refine timing for memory access delays.

### NESDSK
**Description:** Emulates cartridges and memory bank controllers (MBCs). It supports various ROM sizes and mapper types.
**Status:** Implemented
**Next Steps:** Add support for additional mappers (MMC3, MMC5, etc.) and implement save RAM with battery backup.



**Next Steps:** IRQ and NMI 

### NESPPU
**Description:** Emulates the Picture Processing Unit. It generates video output from nametables, attribute tables, palettes, and sprite data.
**Status:** Implemented
**Next Steps:** Implement sprite overflow and sprite zero hit flags, add support for palette RAM reads, and improve rendering accuracy.

### NESAPU
**Description:** Audio Processing Unit (not yet implemented). Will generate sound from the NES's five audio channels.
**Status:** Not Implemented
**Next Steps:** Implement the APU with pulse, triangle, noise, and DMC channels, and integrate audio output via SDL3.
