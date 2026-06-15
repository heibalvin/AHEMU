#include "GBJOY.h"

GBJOY::GBJOY(GBEMU *emu) : GBCOM(emu), buttons(0xFF), select(0x30) {}

// Ensure these three exist in GBJOY.cpp
void GBJOY::powerOn() {
    reset();
}

void GBJOY::powerOff() {
}

void GBJOY::reset() {
    buttons = 0xFF; select = 0x30;
}

void GBJOY::setButton(Button btn, bool pressed) {
    if (pressed) buttons &= ~(1 << btn);
    else         buttons |=  (1 << btn);
}

Uint8 GBJOY::read(Uint16 addr) {
    Uint8 val = select | 0x0F;
    if (!(select & 0x10)) val &= (buttons >> 4); // Read D-Pad
    if (!(select & 0x20)) val &= (buttons & 0x0F); // Read Action
    return val;
}

void GBJOY::write(Uint16 addr, Uint8 value) {
    // Only bits 4 and 5 are writable
    select = (value & 0x30);
}