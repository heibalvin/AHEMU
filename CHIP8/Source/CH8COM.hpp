#pragma once
#include <SDL3/SDL.h>

class CH8EMU;

class CH8COM {
protected:
    CH8EMU* emu = nullptr;

public:
    CH8COM() = default;
    CH8COM(CH8EMU* parentEmu) : emu(parentEmu) {}
    virtual ~CH8COM() = default;

    virtual void powerOn() = 0;
    virtual void powerOff() = 0;
    virtual void reset() = 0;
    virtual void step() {}

    // Decentralized motherboard pipeline access wrappers
    Uint8 readBus(Uint16 address) const;
    void writeBus(Uint16 address, Uint8 value);
};