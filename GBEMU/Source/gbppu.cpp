#include "gbppu.hpp"
#include <SDL3/SDL_stdinc.h>

GBPPU::GBPPU(GBEMU* emu) : GBComponent(emu) {
    // Initialize frame buffers
    for (int i = 0; i < 2; i++) {
        frameBuffers[i] = (Uint8 *)SDL_malloc(width * height * 4); // RGBA format
        SDL_memset(frameBuffers[i], 0, width * height * 4); // Clear frame buffer
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
    return frameBuffers[frameBufferActive];
}

void GBPPU::step() {
    int index = (line * width + dot) * 4;
    int inactiveFrameBuffer = (frameBufferActive + 1) % 2;
    
    frameBuffers[inactiveFrameBuffer][index + 0] = color;
    frameBuffers[inactiveFrameBuffer][index + 1] = color;
    frameBuffers[inactiveFrameBuffer][index + 2] = color;
    frameBuffers[inactiveFrameBuffer][index + 3] = 0xFF;

    color = (color + 1) % 256;

    dot++;
    if (dot >= width) {
        dot = 0;
        line++;
        if (line >= height) {
            // Swap frame buffers
            frameBufferActive = inactiveFrameBuffer;
            isRefreshRequested = true;
            line = 0;
        }
    }
}