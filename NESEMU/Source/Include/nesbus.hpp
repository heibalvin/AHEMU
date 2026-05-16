#ifndef NESBUS_HPP
#define NESBUS_HPP

#include <SDL3/SDL.h>
#include "nescomponent.hpp"

class NESEMU;

class NESBUS : public NESComponent {
public:
    explicit NESBUS(NESEMU* emu);
    ~NESBUS();
    
    Uint8 read(Uint16 address);
    void write(Uint16 address, Uint8 value);

    Uint16 readLE(Uint16 address);
    void writeLE(Uint16 address, Uint16 value);

private:
    Uint8* ioregisters = NULL; // I/O Registers (0xFF00-0xFF7F)
};

#endif // NESBUS_HPP