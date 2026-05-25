#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8BUS : public CH8COM {
public:
    Uint8 RAM[4096]; 

    CH8BUS(CH8EMU* parentEmu) : CH8COM(parentEmu) {
        reset();
    }

    void powerOn() override  { reset(); }
    void powerOff() override { clearRAM(); }
    void reset() override    { clearRAM(); }

    void clearRAM() {
        SDL_memset(RAM, 0, sizeof(RAM));
    }

    // Direct memory lookups with safety bounds fallback guards
    Uint8 read(Uint16 address) const {
        return (address < 4096) ? RAM[address] : 0;
    }

    void write(Uint16 address, Uint8 value) {
        if (address < 4096) {
            RAM[address] = value;
        }
    }
};