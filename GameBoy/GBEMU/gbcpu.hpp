#ifndef GBCPU_HPP
#define GBCPU_HPP

#include <cstdint>

class GBEMU;

class GBCPU {
public:
    GBCPU(GBEMU* emu);

    void step();

private:
    GBEMU* emu;
};

#endif /* GBCPU_HPP */