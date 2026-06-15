// Inside GBJOY.cpp
#include "GBJOY.h"

void GBJOY::write(Uint16 addr, Uint8 value) {
    // Joypad register is at 0xFF00
    // Only bits 4 and 5 are writable
    joyp = (value & 0x30) | (joyp & 0x0F);
}

Uint8 GBJOY::read(Uint16 addr) {
    Uint8 result = joyp & 0x30;
    
    // Logic: 0 = Pressed, 1 = Not Pressed
    if (!(joyp & 0x20)) { // Action buttons
        result |= (btnStates[0] ? 0 : 1) | (btnStates[1] ? 0 : 2) | 
                  (btnStates[2] ? 0 : 4) | (btnStates[3] ? 0 : 8);
    } else if (!(joyp & 0x10)) { // Direction buttons
        result |= (btnStates[4] ? 0 : 1) | (btnStates[5] ? 0 : 2) | 
                  (btnStates[6] ? 0 : 4) | (btnStates[7] ? 0 : 8);
    }
    
    return result | 0xC0; // Bits 6-7 are unused/high
}