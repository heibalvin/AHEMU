#ifndef GBDSK_H
#define GBDSK_H

#include <SDL3/SDL.h>
#include "GBCOM.h"

class GBDSK: public GBCOM {
private:
    Uint8 *romData;
    size_t romSize;

    // MBC State
    Uint8 romBank;       // Selected ROM bank (1-N)
    Uint8 ramBank;       // Selected RAM bank (0-3)
    bool  ramEnabled;    // RAM accessibility
    bool  bankingMode;   // false = ROM Banking Mode, true = RAM Banking Mode
    
    Uint8 externalRAM[0x8000]; // 32KB SRAM buffer

public:
    GBDSK(GBEMU *emu);
    ~GBDSK();

    void powerOn() override;
    void powerOff() override;
    void reset() override;

    Uint8 read(Uint16 addr) override;
    void  write(Uint16 addr, Uint8 value) override;

    Uint8 readRam(Uint16 addr);
    void  writeRam(Uint16 addr, Uint8 value);
    void load(const Uint8* data, size_t size);
};

#endif