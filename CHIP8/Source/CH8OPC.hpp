#pragma once
#include <SDL3/SDL.h>

class CH8CPU;

struct CH8OPC {
    const char* mnemonic;
    Uint16 mask;
    Uint16 id;
    void (CH8CPU::*handler)();
    Uint8 length;
    Uint8 cycles;
};