#ifndef GBJOY_H
#define GBJOY_H

#include <SDL3/SDL.h>
// Inside GBJOY.h
#include "GBCOM.h"

class GBJOY : public GBCOM {
    Uint8 joyp;
    bool btnStates[8];

public:
    explicit GBJOY(GBEMU *emu) : GBCOM(emu), joyp(0xCF) {} // Initialize with default mask

    // GBCOM Interface implementation
    void powerOn() override { joyp = 0xCF; }
    void powerOff() override {}
    void reset() override { powerOn(); }

    Uint8 read(Uint16 addr) override;
    void  write(Uint16 addr, Uint8 value) override;

    // Component-specific methods
    void setKeyState(int key, bool pressed);
};

#endif 	// GBJOY_H