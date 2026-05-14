#ifndef GBDSK_HPP
#define GBDSK_HPP

#include <cstdint>

class GBEMU;

class GBDSK {
public:
    GBDSK(GBEMU* emu);

    void step();

private:
    GBEMU* emu;
};

#endif /* GBDSK_HPP */