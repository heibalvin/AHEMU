#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8PPU : public CH8COM {
public:
    const int CH8_WIDTH  = 64;
    const int CH8_HEIGHT = 32;

    // Fully migrated to use clean SDL3 Uint32 array buffers
    Uint32 frameBuffer[64 * 32];

    CH8PPU(CH8EMU* parentEmu) : CH8COM(parentEmu) {
        reset();
    }

    void powerOn() override {
        reset();
    }

    void powerOff() override {
        reset();
    }

    void reset() override {
        clearScreen();
    }

    void clearScreen() {
        SDL_memset(frameBuffer, 0, sizeof(frameBuffer));
    }

    /**
     * Draws a sprite using native SDL3 sizing primitives.
     */
    Uint8 writeSprite(Uint8 xCoordinate, Uint8 yCoordinate, const Uint8* spriteDataPointer, Uint8 spriteHeight) {
        Uint8 collisionDetected = 0;

        Uint8 startX = xCoordinate % CH8_WIDTH;
        Uint8 startY = yCoordinate % CH8_HEIGHT;

        for (int row = 0; row < spriteHeight; row++) {
            if (startY + row >= CH8_HEIGHT) break;

            Uint8 spriteByte = spriteDataPointer[row];

            for (int col = 0; col < 8; col++) {
                if (startX + col >= CH8_WIDTH) break;

                if ((spriteByte & (0x80 >> col)) != 0) {
                    int targetPixelIndex = (startX + col) + ((startY + row) * CH8_WIDTH);

                    if (frameBuffer[targetPixelIndex] == 0xFFFFFFFF) {
                        frameBuffer[targetPixelIndex] = 0x00000000;
                        collisionDetected = 1;
                    } else {
                        frameBuffer[targetPixelIndex] = 0xFFFFFFFF;
                    }
                }
            }
        }

        return collisionDetected;
    }
};