#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8CON : public CH8COM {
public:
    Uint8 KEYS[16];

    CH8CON(CH8EMU* parentEmu);
    ~CH8CON() = default;

    void powerOn() override;
    void powerOff() override;
    void reset() override;

    void clearKeys();
    Uint8 isKeyPressed(Uint8 keyIndex) const;
    void setKeyState(Uint8 keyIndex, Uint8 isPressed);
};