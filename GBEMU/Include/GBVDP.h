#ifndef GBVDP_H
#define GBVDP_H

#include <SDL3/SDL.h>
#include "GBCOM.h"


struct Pixel {
    Uint8 color;
    Uint8 palette;
    bool priority;
};

enum FetcherState {
    FETCH_TILE_ID,
    FETCH_TILE_DATA0,
    FETCH_TILE_DATA1,
    PUSH_TO_FIFO
};

class GBVDP : public GBCOM {
private:
    Uint8 VRAM[0x2000];
    Uint8 OAM[0xA0];
    Uint8 LX;
    Uint8 LCDC, STAT, LY;
    Uint8 SCX, SCY; // 0xFF43, 0xFF42
    Uint8 BGP; // 0xFF47: Background Palette Data
    
    // Store only the OAM indices (0-39) of the sprites found for this line
    Uint8 spriteIndices[10]; 
    int spriteCount;

    // Window layer
    Uint8 WX, WY;          // Window X and Y registers (mapped from 0xFF4A, 0xFF4B)
    Uint8 windowLine;      // Internal counter, only increments when window draws

    // OAM DMA 
    Uint8 dmaSource; // The high byte (e.g., 0xC0 if copying from 0xC000)
    bool dmaActive;
    int dmaCycles;   // To track the 160-byte transfer duration

    // A standard 4-shade grayscale palette in RGBA (0xAABBGGRR)
    const Uint32 PALETTE[4] = {
        0xFFFFFFFF, // Color 0: White
        0xFFAAAAAA, // Color 1: Light Gray
        0xFF555555, // Color 2: Dark Gray
        0xFF000000  // Color 3: Black
    };

    // Double Buffering: 0 and 1
    Uint32 buffers[2][160 * 144];
    Uint32* frontBuffer;
    int backBufferIdx;

    // FIFO implementation
    #define FIFO_SIZE 16
    Pixel pixelFIFO[FIFO_SIZE];
    int fifoReadIdx;
    int fifoWriteIdx;
    int fifoCount;

    // Cycle tracking
    Uint32 cycle;

public:
    explicit GBVDP(GBEMU* emu);

    void powerOn() override { reset(); }
    void powerOff() override {}
    void reset() override;

    Uint8 read(Uint16 addr) override;
    void  write(Uint16 addr, Uint8 value) override;
    
    void  update(Uint8 cycles);
    void renderPixel();
    Uint8 getBackgroundOrWindowPixel();
    Uint8 fetchSpritePixel(Uint8 idx, Uint8 &sFlags);
    void processSpriteMixing(Uint8 bgIdx, Uint32 &finalColor);
    void oamSearch();
    void checkSTATInterrupts();

    void swapBuffers();
    const Uint32* getFrontBuffer() const { return frontBuffer; }

    void pushPixel(Pixel p);
    Pixel popPixel();
};

#endif