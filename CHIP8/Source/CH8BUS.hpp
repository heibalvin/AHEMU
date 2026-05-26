#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8BUS : public CH8COM {
public:
    Uint8 RAM[4096]; 
    
    CH8BUS(CH8EMU* parentEmu);
    ~CH8BUS() = default;

    void powerOn() override;
    void powerOff() override;
    void reset() override;

    void clearRAM();
    Uint8 read(Uint16 address) const;
    void write(Uint16 address, Uint8 value);
};