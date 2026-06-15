#ifndef GBJOY_H
#define GBJOY_H

#include <SDL3/SDL.h>
#include "GBCOM.h"

class GBJOY : public GBCOM {
private:
    Uint8 buttons;   // Bitmask: 0=pressed, 1=released
    Uint8 select;    // Current selection (bits 4-5 of 0xFF00)

public:
    explicit GBJOY(GBEMU *emu);
    void powerOn() override;
    void powerOff() override;
    void reset() override;

    enum Button { A, B, SELECT, START, RIGHT, LEFT, UP, DOWN };
    void setButton(Button btn, bool pressed);

    Uint8 read(Uint16 addr) override;
    void  write(Uint16 addr, Uint8 value) override;
};

#endif