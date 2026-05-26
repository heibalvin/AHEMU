# CHIP-8 Emulator

## Description
This is a CHIP-8 emulator written in modern C++ (C++17) using SDL3 for graphics, input, and audio. The emulator accurately replicates the behavior of the original CHIP-8 interpreter, allowing users to run classic CHIP-8 games and applications.

## How to Build and Run

### Prerequisites
- **g++** (C++17-capable compiler)
- **SDL3** (install via Homebrew: `brew install sdl3` or your system's package manager)
- `make`

### Compilation
```bash
make
```
The binary will be generated at `./Build/chip8emu`.

### Running the Emulator
```bash
# Build and run with the default ROM (if any)
make run

# Run a specific ROM file
./Build/chip8emu <path/to/rom.ch8>
```

### Other Useful Targets
| Command          | Description                              |
|------------------|------------------------------------------|
| `make clean`     | Remove all build artifacts               |
| `make doxygen`   | Generate Doxygen documentation (see Docs/) |

## Repository
This project is hosted on GitHub at: https://github.com/heibalvin/AHEMU
The CHIP-8 emulator source code is located in the `CHIP8/` directory.

## High Level Software Architecture

The emulator is structured into several modular components, each responsible for a specific aspect of the CHIP-8 system:

### Core Components
1. **SDLEMU** (SDL3 Integration Layer)
   - Creates and manages SDL window, renderer, and textures
   - Handles event polling and rendering at target FPS
   - Displays splash screen during initialization

2. **CH8EMU** (Main Emulator Class)
   - Coordinates all subsystems (CPU, memory, display, input, audio)
   - Handles timing synchronization (FPS/UPS)
   - Provides the main update loop

3. **CH8CPU** (Central Processing Unit)
   - Implements the fetch-decode-execute cycle
   - Manages registers (V, I, PC, SP), timers, and stack
   - Contains opcode handler table for all 35 CHIP-8 instructions

4. **CH8PPU** (Pixel Processing Unit)
   - Renders graphics to a 64x32 pixel display
   - Handles sprite drawing and collision detection
   - Manages the framebuffer

5. **CH8BUS** (System Bus)
   - Provides 4KB memory space for ROM, RAM, and hardware registers
   - Handles memory read/write operations

6. **CH8KEY** (Keyboard Controller)
   - Maps CHIP-8 hex keypad (0x0-0xF) to host keyboard
   - Handles key press/release events
   - Implements the FX0A blocking key wait opcode

7. **CH8APU** (Audio Processing Unit)
   - Generates beep sound when sound timer is active
   - Uses SDL audio subsystem for tone generation

8. **CH8DSK** (Disk System)
   - Loads ROM files into memory
   - Provides interface for inserting new ROMs

9. **CH8COM** (Common Communication Interface)
   - Abstract base class defining standard hardware interface
   - All hardware components inherit from this class
   - Defines powerOn(), powerOff(), reset(), and step() methods

### Data Flow
1. SDLEMU handles SDL events and rendering at the target refresh rate
2. The main loop in `CH8EMU::update()` advances time and triggers CPU steps
3. CPU fetches opcode from memory via CH8BUS
4. CPU decodes and executes opcode, potentially modifying:
   - Registers (stored in CPU)
   - Memory (via CH8BUS)
   - Display state (via CH8PPU)
   - Audio state (via CH8APU)
   - Input state (via CH8KEY)
5. SDL handles audio output based on CH8APU state
6. SDL handles input events which are processed by CH8KEY

### Key Features
- Cycle-accurate CPU emulation
- Original CHIP-8 instruction set (35 opcodes)
- 60 FPS video output with configurable scaling
- Beep sound emulation
- ROM loading and hot-swapping
- Cross-platform (macOS, Linux, Windows with SDL3)
- Comprehensive Doxygen documentation

## References
- [CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite) — ROM tests for verifying emulator correctness.
- [CHIP-8 Emulator (C++)](https://austinmorlan.com/posts/chip8_emulator/) — A helpful guide for building a CHIP-8 emulator in C++.

### Keypad Mapping
The emulator maps the original CHIP-8 16-key keypad to the keyboard as follows:

| CHIP-8 key | Host key |
|------------|----------|
| 1          | 1        |
| 2          | 2        |
| 3          | 3        |
| C          | C        |
| 4          | 4        |
| 5          | 5        |
| 6          | 6        |
| D          | D        |
| 7          | 7        |
| 8          | 8        |
| 9          | 9        |
| E          | E        |
| A          | A        |
| 0          | 0        |
| B          | B        |
| F          | F        |
