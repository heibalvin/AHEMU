#ifndef GBCPU_HPP
#define GBCPU_HPP

#include <SDL3/SDL.h>
#include "gbcomponent.hpp"

class GBEMU;

class GBCPU : public GBComponent {
public:
    explicit GBCPU(GBEMU* emu);
    ~GBCPU();

    void step() override;

private:
    friend class GBEMU;
    friend class GBBUS;

    Uint8 *hram = NULL; // High RAM (HRAM) for the last 128 bytes of memory
    Uint8 *wrams[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    int wramCount = 1;
    int wramActive[2] = { 0, 0 };
};

#endif /* GBCPU_HPP */