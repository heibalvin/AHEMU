#include "CH8PPU.hpp"

CH8PPU::CH8PPU(CH8EMU* parentEmu) : CH8COM(parentEmu) {
    reset();
}

void CH8PPU::powerOn() {
    reset();
}

void CH8PPU::powerOff() {
    reset();
}

void CH8PPU::reset() {
    clearScreen();
}

void CH8PPU::clearScreen() {
    SDL_memset(frameBuffer, 0, sizeof(frameBuffer));
}

Uint8 CH8PPU::writeSprite(Uint8 xCoordinate, Uint8 yCoordinate, const Uint8* spriteDataPointer, Uint8 spriteHeight) {
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