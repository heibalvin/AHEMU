#include "nesppu.hpp"
#include <SDL3/SDL_stdinc.h>
#include "nesemu.hpp"
#include "nesdsk.hpp"
#include "nescpu.hpp"

NESPPU::NESPPU(NESEMU* emu) : NESComponent(emu) {
    // Initialize frame buffers with red
    for (int i = 0; i < 2; i++) {
        frameBuffers[i] = (Uint8 *)SDL_malloc(width * height * 4); // RGBA format
        SDL_memset(frameBuffers[i], 0xFF, width * height * 4); // Fill with white (255, 255, 255, 255)
    }

    frameBufferActive = 0;
    isRefreshRequested = false;

    // Initialise palette RAM
    palRam = (Uint8 *)SDL_malloc(8 * 4);        // 8 palettes of 4 index for background and sprite palettes
    SDL_memset(palRam, 0, 8 * 4);               // Clear palette RAM

    // Initialise OAM for 40 sprites
    oamRam = (Uint8 *)SDL_malloc(64 * 4);       // 64 sprites * 4 bytes each
    SDL_memset(oamRam, 0, 64 * 4);              // Clear OAM

    // Initialise 4KB VRAM
    vram = (Uint8 *)SDL_malloc(4 * 1024);         // 4KB of VRAM
    SDL_memset(vram, 0, 4 * 1024);                // Clear VRAM
}

NESPPU::~NESPPU() {
    for (int i = 0; i < 2; i++) {
        if (frameBuffers[i]) {
            SDL_free(frameBuffers[i]);
            frameBuffers[i] = NULL;
        }
    }
    if (oamRam) {
        SDL_free(oamRam);
        oamRam = NULL;
    }
    if (palRam) {
        SDL_free(palRam);
        palRam = NULL;
    }
    if (vram) {
        SDL_free(vram);
        vram = NULL;
    }
}

void NESPPU::powerOn() {
    SDL_Log("NESPPU: powerOn()...");
// 1. Hard power-up clears control registers completely
    ppu_ctrl = 0x00;
    ppu_mask = 0x00;
    
    // Bit 7 is cold-booted as 0. Often bits 5 or 6 can contain stale hardware noise,
    // but 0x00 is the safest starting point for emulators.
    ppu_status = 0x00;

    // 2. Initialize internal location coordinates
    cycles = 0;
    scanline = 0;
    dot = 0;
    line = 0;
    color = 0;

    // 3. Reset hardware frame pointers
    frameBufferActive = 0;
    isRefreshRequested = false;

    // 4. Wipe internal memory domains to ensure no residual data
    if (palRam) SDL_memset(palRam, 0, 8 * 4);
    if (oamRam) SDL_memset(oamRam, 0, 64 * 4);
    if (vram)   SDL_memset(vram, 0, 4 * 1024);
}

void NESPPU::reset() {
    SDL_Log("NESPPU: reset()...");

    // 1. Control and Mask registers are forcefully cleared on warm reset
    ppu_ctrl = 0x00;
    ppu_mask = 0x00;

    // 2. Clear V-Blank flag (Bit 7), Sprite 0 Hit (Bit 6), and Sprite Overflow (Bit 5).
    // We mask with 0x1F to leave any remaining lower bits untouched.
    ppu_status &= 0x1F;

    // 3. Bring the scanning alignment back to the top of the display pipeline frame
    cycles = 0;
    scanline = 0;
}

const Uint8 *NESPPU::getFrameBuffer() const {
    return frameBuffers[(frameBufferActive + 1) % 2]; // Return the non-active buffer for rendering
}

Uint8 NESPPU::read(Uint16 address) {
    // 1. Check if the read request is targeting CPU memory-mapped registers ($2000-$3FFF)
    // The 8 hardware registers are mirrored every 8 bytes across this region.
    Uint16 regOffset = address % 8;

    switch (regOffset) {
        case 2: { // $2002 PPUSTATUS
            Uint8 temp = ppu_status;
            // CRITICAL SIDE EFFECT: Reading PPUSTATUS clears bit 7 (V-Blank status flag)
            // and resets the internal PPU scroll/address latch flip-flop.
            ppu_status &= ~0x80; 
            return temp;
        }
        case 7: { // $2007 PPUDATA
            // Reading from PPUDATA retrieves bytes out of PPU VRAM.
            // TBD: Implement VRAM buffered read sequencing here later.
            return 0x00;
        }
        default:
            // Most other PPU registers ($2000, $2001, $2003, $2004, $2005, $2006) are write-only.
            // Reading them typically returns an open-bus data state or 0.
            return 0x00;
    }
}

void NESPPU::write(Uint16 address, Uint8 value) {
    Uint16 regOffset = address % 8;

    switch (regOffset) {
        case 0: // $2000 PPUCTRL
            ppu_ctrl = value;
            // If the CPU enables NMIs while the PPU is already in a V-Blank period,
            // an NMI is generated immediately.
            if ((ppu_ctrl & 0x80) && (ppu_status & 0x80)) {
                emu->cpu->nmi_asserted = true;
            }
            break;

        case 1: // $2001 PPUMASK
            ppu_mask = value;
            break;

        case 2: // $2002 PPUSTATUS (Hardware Read-Only, writes ignored)
            break;

        case 5: // $2005 PPUSCROLL
            // TBD: Feed scroll offsets into internal registers (X/Y latching)
            break;

        case 6: // $2006 PPUADDR
            // TBD: Sequence high byte then low byte to update the current VRAM pointer address
            break;

        case 7: // $2007 PPUDATA
            // TBD: Write value directly to PPU VRAM via the current internal PPUADDR register pointer, 
            // then automatically increment the register pointer by either 1 or 32 based on PPUCTRL.
            break;

        default:
            break;
    }
}

void NESPPU::step() {
    // 1. Advance the fine internal clock components
    cycles++; // Cycle acts as the current horizontal dot (0-340)
    if (cycles >= 341) {
        cycles = 0;
        scanline++; // Progress to next vertical scanline (0-261)
        
        if (scanline >= 262) {
            scanline = 0;
            isRefreshRequested = true;
            frameBufferActive = (frameBufferActive + 1) % 2;

            // Frame is completely done rendering!
            emu->raiseEvent(NESEvent::FRAME_COMPLETE);
        }
    }

    // 2. Check Scanline Boundaries for Interrupt Processing
    if (scanline == 241 && cycles == 1) {
        // Set the V-Blank Flag inside our status register representation (Bit 7)
        ppu_status |= 0x80;

        // Signal VBlank arrival autonomously
        emu->raiseEvent(NESEvent::VBLANK_START);
        
        // If Bit 7 of PPUCTRL ($2000) is set, generate a hard hardware NMI signal!
        if ((ppu_ctrl & 0x80) != 0) {
            emu->cpu->nmi_asserted = true; 
            SDL_Log("NESPPU: V-Blank reached on line 241. Triggering CPU NMI!");
        }
    }

    if (scanline == 261 && cycles == 1) {
        // Clear the V-Blank flag on the pre-render scanline preparation loop
        ppu_status &= ~0x80;
    }

    // 3. Keep your existing procedural test pattern generation drawing safely tied to the grid:
    if (scanline < height && cycles < width) {
        int index = (scanline * width + cycles) * 4;
        frameBuffers[frameBufferActive][index + 0] = color; // Red
        frameBuffers[frameBufferActive][index + 1] = 0x00;  // Green
        frameBuffers[frameBufferActive][index + 2] = color; // Blue
        frameBuffers[frameBufferActive][index + 3] = 0xFF;  // Alpha
    }

    // Color shifting sequence loop wrapper 
    dot++;
    if (dot >= width) {
        dot = 0;
        line++;
        if (line >= height) {
            line = 0;
            color = (color + 1) % 256;
        }
    }
}

// void NESPPU::experimental() {
//     if 
// }