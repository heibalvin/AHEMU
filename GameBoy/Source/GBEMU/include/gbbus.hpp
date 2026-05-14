#ifndef GBBUS_HPP
#define GBBUS_HPP

#include <SDL3/SDL.h>
#include "gbcomponent.hpp"

class GBEMU;

class GBBUS : public GBComponent {
public:
    explicit GBBUS(GBEMU* emu);
    Uint8 read8(Uint16 address);
    void write8(Uint16 address, Uint8 value);
};

#endif // GBBUS_HPP