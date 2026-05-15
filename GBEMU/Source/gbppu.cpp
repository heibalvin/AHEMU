#include "gbppu.hpp"
#include <SDL3/SDL_stdinc.h>

GBPPU::GBPPU(GBEMU* emu) : GBComponent(emu) {
    // Initialize frame buffers with red
    for (int i = 0; i < 2; i++) {
        frameBuffers[i] = (Uint8 *)SDL_malloc(width * height * 4); // RGBA format
        SDL_memset(frameBuffers[i], 0xFF, width * height * 4); // Fill with white (255, 255, 255, 255)
    }

    frameBufferActive = 0;
    isRefreshRequested = false;

    // Initialise OAM for 40 sprites
    oam = (Uint8 *)SDL_malloc(40 * 4); // 40 sprites * 4 bytes each
    SDL_memset(oam, 0, 40 * 4); // Clear OAM

    // Initialise 8KB VRAM
    vramCount = 1;
    for (int i = 0; i < vramCount; i++) {
        vrams[i] = (Uint8 *)SDL_malloc(0x2000); // 8KB of VRAM
        SDL_memset(vrams[i], 0, 0x2000); // Clear VRAM
    }
    vramActive = 0;
}

GBPPU::~GBPPU() {
    for (int i = 0; i < 2; i++) {
        if (frameBuffers[i]) {
            SDL_free(frameBuffers[i]);
            frameBuffers[i] = NULL;
        }
    }
    if (oam) {
        SDL_free(oam);
        oam = NULL;
    }
    for (int i = 0; i < vramCount; i++) {
        if (vrams[i]) {
            SDL_free(vrams[i]);
            vrams[i] = NULL;
        }
    }
}

const Uint8 *GBPPU::getFrameBuffer() {
    return frameBuffers[(frameBufferActive + 1) % 2]; // Return the non-active buffer for rendering
}

void GBPPU::step() {
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