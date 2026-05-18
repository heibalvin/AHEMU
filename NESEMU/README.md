# SDL3 NES Emulator (NESEMU)

## Table of Contents
- [Repo Structure](#repo-structure)
- [Install & Execution](#install--execution)
- [Debug & Stepping Modes](#debug--stepping-modes)
- [Component Interaction Architecture](#component-interaction-architecture)
- [Subsystem Status & Roadmap](#subsystem-status--roadmap)

---

## Repo Structure

The repository isolates cross-platform desktop host layers from core emulated console silicon:
* **Build / Output**: Houses intermediate compilation object data (`.o`) and final binary executables.
* **Resources**: Contains target configuration assets and standard `.nes` format game cartridge images.
* **Source**: Implementation files (`.cpp`/`.hpp`) tracking individual physical NES architecture components.
* **Makefile**: Single-point builder engine managing stable compilation variables against system hooks.

---

## Install & Execution

Instructions to install dependencies via Homebrew:
```bash
brew install sdl3  # Target stable distribution verified: 3.4.8

```

To build and launch the emulator platform:

```bash
make       # Compiles processing units, bus lines, and presentation frontends
make run   # Spins up the graphic context and boots the assigned cartridge image

```

---

## Debug & Stepping Modes

The framework utilizes an **Autonomous Event-Driven Debugging Model**. Rather than forcing the main thread loop to blindly poll sub-units, the internal hardware sub-components (`NESCPU`, `NESPPU`) step naturally and autonomously halt the execution line by signaling milestones back up to the console core via an event gate interface.

You can configure what explicit hardware milestone the engine should pause on using the keyboard. Pressing the **Spacebar** resumes autonomous clock ticking until that chosen target flag condition trips.

### Hardware Milestone Hotkeys

Configure the active structural event listener at runtime using the following hotkeys:

* **`C` (Cycle Step)**: Halts the system instantly upon completion of exactly **1 master clock cycle pass**.
* **`I` (Instruction Step)**: Runs components continuously until the CPU completes its current operation opcode, pausing directly at the next opcode fetch phase (`waitCycles == 0`). Ideal for reviewing changes to registers ($A, X, Y, PC, P, SP$).
* **`V` (V-Blank Start)**: Unrolls the clock rapidly until the PPU hits scanline 241, dot 1, which sets the vertical blanking flag status bit inside the `$2002` register.
* **`F` (Frame Complete) [Default]**: Resolves execution loops cleanly until an entire 262-scanline frame matrix is evaluated and the frame buffer pointers swap. This mode matches normal frame rate updates.
* **`SPACEBAR` (Execute to Halt)**: Dispatches the clock loop runner to advance components asynchronously until your designated milestone is triggered.

---

## Component Interaction Architecture

The architectural loop strictly mirrors the physical traces and address bus lines layout found on a real hardware NES console motherboard:

```
[ SDLEMU Window / Input / Audio ]
              │  ▲
   Pushes keys│  │ Pulls Frame Buffer
              ▼  │
          [ NESEMU Core ]
              │  ▲
              ▼  │ Component Tick Dispatch
     ┌────────┴──┴────────┐
     ▼                    ▼
 [ NESCPU ]           [ NESPPU ]
     │  ▲                 │  ▲
     │  │ Read / Write    │  │ Memory Access
     ▼  │                 ▼  │
 ┌───────────────────────────┴┐
 │          NESBUS            │
 └────────────────────────────┘
     │            │         │
     ▼            ▼         ▼
 [  WRAM  ]   [ CART ROM ] [ PPU Regs ]
 (CPU Direct)  (NESDSK)   ($2000-$2007)

```

### Data Highway & Signal Tracing

#### 1. Core Clock Pumping (`NESEMU`)

Responsible for timing execution. For every **1 CPU Clock Cycle** executed (`cpu->step()`), the Picture Processing Unit must step exactly **3 times** (`ppu->step()`). `NESEMU` monitors an incoming portal `raiseEvent()` where components broadcast limits autonomously to halt execution lines.

#### 2. Polymorphic Routing Bridge (`NESBUS`)

Maintains data integrity by enforcing abstraction barriers. Components do not directly tap or adjust parameters inside neighbor modules. Memory actions map through `NESBUS`:

* **`$0000 - $1FFF`**: Handed off to `NESCPU::read()` / `write()` to process the 2KB local WRAM mirrors.
* **`$2000 - $3FFF`**: Handed off to `NESPPU::read()` / `write()`. This isolates the 8 primary I/O memory-mapped registers (`$2000` to `$2007`, mirrored every 8 bytes) away from the parallel PPU video bus layout.
* **`$4020 - $FFFF`**: Routed straight to `NESDSK::read()` / `write()` to retrieve game instructions or run memory mapper bank switching operations.

#### 3. Asynchronous Hardware Interrupts (NMI)

When the scanning beam hits scanline 241, `NESPPU` flips bit 7 of its internal `$2002` (`PPUSTATUS`) tracker. If bit 7 of the game-configured `$2000` (`PPUCTRL`) register is active, the PPU flags the CPU variable directly (`emu->cpu->nmi_asserted = true`). On the next instruction boundary pass, the CPU captures the interrupt line condition, executes an automated `NMIInterrupt()` stack hijack sequence, and logs an autonomous `NESEvent::NMI_TRIGGERED` milestone signal.

---

## Subsystem Status & Roadmap

### SDLEMU

* **Description:** Cross-platform shell wrapper parsing file data streams, initializing host execution window pipelines, generating hardware graphics textures, and capturing host keyboard triggers.
* **Status:** Complete for File ingestion, Presentation Layer Texture Blitting, and Debug Event Hook mappings.
* **Next Steps:** Bind host buttons to standard physical NES Controller buttons; hook up SDL Audio stream threads.

### NESCPU

* **Description:** Emulation engine of the central MOS 6502 processing unit (omitting BCD decimal calculations). Tracks instruction fetch loops, breaks down operations decoding, performs addressing mode index math, and balances execution flags.
* **Status:** Main opcodes and cycle wait timing sequences are stable.
* **Next Steps:** Evaluate instruction timing states against nested automated validation suite ROM programs.

### NESPPU

* **Description:** Custom Picture Processing Unit emulation matrix. Tracks scanlines, flips V-Blank control state registers, and maps isolated color palettes.
* **Status:** Polymorphic I/O memory register access points isolated; basic frame-interval steps active.
* **Next Steps:** Transition from a procedural test pattern array to real background layout tileset address data decoding.

### NESBUS

* **Description:** Motherboard signal dispatcher. Maps component address lines using uniform polymorphic access protocols.
* **Status:** Fully integrated with the new memory model architecture.
* **Next Steps:** Add safety fallbacks to account for unmapped or open-bus floating memory space reads.

### NESDSK

* **Description:** Cartridge container wrapper parsing structural iNES format files, splitting code pages (PRG-ROM), and mapping graphics tiles arrays (CHR-ROM).
* **Status:** Game loading and core Mapper 0 (NROM) configurations are operational.
* **Next Steps:** Add support for complex memory mappers (such as MMC1 and MMC3).

### NESAPU

* **Description:** Audio Processing Unit synthesizer simulating sound generation layers.
* **Status:** Not Implemented.
* **Next Steps:** Build baseline wave generator arrays to service square wave audio outputs.
