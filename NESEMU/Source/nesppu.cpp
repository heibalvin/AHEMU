#include "nesppu.hpp"
#include <SDL3/SDL_stdinc.h>
#include "nesemu.hpp"
#include "nesdsk.hpp"

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

    // Initialise PPU I/O registers
    registers = (Uint8 *)SDL_malloc(8);          // 8 registers (0x2000-0x2007)
    SDL_memset(registers, 0, 8);                // Clear registers
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
    if (registers) {
        SDL_free(registers);
        registers = NULL;
    }
}

void NESPPU::powerOn() {
    SDL_Log("NESPPU: powerOn()...");

    // Reset PPU state
    cycles = 0;
    scanline = 0;
    dot = 0;
    line = 0;
    color = 0;

    // Clear frame buffers
    for (int i = 0; i < 2; i++) {
        SDL_memset(frameBuffers[i], 0xFF, width * height * 4); // Fill with white (255, 255, 255, 255)
    }

    isRefreshRequested = false;
}

const Uint8 *NESPPU::getFrameBuffer() const {
    return frameBuffers[(frameBufferActive + 1) % 2]; // Return the non-active buffer for rendering
}

void NESPPU::step() {
    // SDL_Log("NESPPU: Dot: %d, Line: %d, Color: %d", dot, line, color);
    int index = (line * width + dot) * 4;
    
    frameBuffers[frameBufferActive][index + 0] = color; // Red
    frameBuffers[frameBufferActive][index + 1] = color; // Green
    frameBuffers[frameBufferActive][index + 2] = color; // Blue
    frameBuffers[frameBufferActive][index + 3] = 0xFF;  // Alpha

    dot++;
    if (dot >= width) {
        dot = 0;
        line++;
        if (line >= height) {
            line = 0;
            dot = 0;
            color = (color + 1) % 256;

            // Switch to the cleared buffer for the next frame
            frameBufferActive = (frameBufferActive + 1) % 2;
            isRefreshRequested = true;
            // SDL_Log("NESPPU: Frame completed, switching buffers. Refresh requested.");
        }
    }
}

Uint8 NESPPU::read(Uint16 address) {
    if (address >= 0x0000 && address < 0x1FFF) {
        return emu->dsk->chrRoms[0][address];
    } else if (address >= 0x2000 && address <= 0x2FFF) {
        return vram[address - 0x2000];
    } else if (address >= 0x3F00 && address <= 0x3FFF) {
        return palRam[(address - 0x3F00) % 0x0020];
    }

    return 0x00;
}

void NESPPU::write(Uint16 address, Uint8 value) {
    if (address >= 0x0000 && address < 0x1FFF) {
        emu->dsk->chrRoms[0][address] = value;
    } else if (address >= 0x2000 && address <= 0x2FFF) {
        vram[address - 0x2000] = value;
    } else if (address >= 0x3F00 && address <= 0x3FFF) {
        palRam[(address - 0x3F00) % 0x0020] = value;
    }
}

void NESPPU::experimental() {
    SDL_Log("NESPPU: Experimental function called. Dot: %d, Line: %d", dot, line);
    
    // if (cycle % 8 == 0) {
    //     nameTableByte = vram[];
    // }
    

    cycles += 1; // Increment cycle count
}