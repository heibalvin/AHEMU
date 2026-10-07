#ifndef GBBUS_H
#define GBBUS_H

#include <SDL3/SDL.h>
#include "GBCOM.h"

class GBEMU;

class GBBUS : public GBCOM {
private:
    Uint8 WRAM[0x2000]; // 8KB

public:
    explicit GBBUS(GBEMU &emu);
    ~GBBUS(); // Explicitly declare here

    // GBCOM Interface
    void powerOn() override { reset(); }
    void powerOff() override {}
    void reset() override;

    // Bus Access
    Uint8 read(Uint16 addr) override;
    void  write(Uint16 addr, Uint8 value) override;

    // Endian-Aware Accessors
    Uint16 readLE(Uint16 addr);
    void   writeLE(Uint16 addr, Uint16 value);
    Uint16 readBE(Uint16 addr);
    void   writeBE(Uint16 addr, Uint16 value);
};

#endif