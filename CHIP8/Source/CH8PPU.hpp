#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8PPU : public CH8COM {
public:
    const int CH8_WIDTH  = 64;
    const int CH8_HEIGHT = 32;

    Uint32 frameBuffer[64 * 32];

    CH8PPU(CH8EMU* parentEmu);
    ~CH8PPU() = default;

    void powerOn() override;
    void powerOff() override;
    void reset() override;

    void clearScreen();
    Uint8 writeSprite(Uint8 xCoordinate, Uint8 yCoordinate, const Uint8* spriteDataPointer, Uint8 spriteHeight);
};