#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8CON : public CH8COM {
public:
    // Fully migrated to use standard SDL3 Uint8 registers
    Uint8 KEYS[16];

    CH8CON(CH8EMU* parentEmu) : CH8COM(parentEmu) {
        reset();
    }

    void powerOn() override {
        reset();
    }

    void powerOff() override {
        reset();
    }

    void reset() override {
        clearKeys();
    }

    void clearKeys() {
        SDL_memset(KEYS, 0, sizeof(KEYS));
    }

    Uint8 isKeyPressed(Uint8 keyIndex) const {
        return (keyIndex < 16) ? KEYS[keyIndex] : 0;
    }

    void setKeyState(Uint8 keyIndex, Uint8 isPressed) {
        if (keyIndex < 16) {
            KEYS[keyIndex] = (isPressed ? 1 : 0);
        }
    }
};