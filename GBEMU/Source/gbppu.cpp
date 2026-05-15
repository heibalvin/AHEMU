#include "gbppu.hpp"
#include <SDL3/SDL_stdinc.h>

GBPPU::GBPPU(GBEMU* emu) : GBComponent(emu) {
    // Initialize frame buffers
    frameBuffers = new Uint8*[2];
    for (int i = 0; i < 2; i++) {
        frameBuffers[i] = (Uint8 *)SDL_malloc(width * height * 4); // RGBA format
        SDL_memset(frameBuffers[i], 0, width * height * 4); // Clear frame buffer
    }
    activeFrameBuffer = 0;
    isRefreshRequested = false;
}

GBPPU::~GBPPU() {
    for (int i = 0; i < 2; i++) {
        SDL_free(frameBuffers[i]);
    }
    delete[] frameBuffers;
}

const Uint8 *GBPPU::getFrameBuffer() {
    return frameBuffers[activeFrameBuffer];
}

void GBPPU::step() {
    int index = (line * width + dot) * 4;

    frameBuffers[activeFrameBuffer][index + 0] = color;
    frameBuffers[activeFrameBuffer][index + 1] = color;
    frameBuffers[activeFrameBuffer][index + 2] = color;
    frameBuffers[activeFrameBuffer][index + 3] = 0xFF;

    color = (color + 1) % 256;

    dot++;
    if (dot >= width) {
        dot = 0;
        line++;
        if (line >= height) {
            // Swap frame buffers
            activeFrameBuffer = (activeFrameBuffer + 1) % 2;
            isRefreshRequested = true;
            line = 0;
        }
    }
}