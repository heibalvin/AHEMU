#ifndef GBCPU_HPP
#define GBCPU_HPP

#include <SDL3/SDL.h>
#include "gbcomponent.hpp"

class GBEMU;

class GBCPU : public GBComponent {
public:
    explicit GBCPU(GBEMU* emu);

    void step() override;
private:
    friend class GBEMU;
    friend class GBBUS;
};

#endif /* GBCPU_HPP */