#ifndef GBDSK_HPP
#define GBDSK_HPP

#include <SDL3/SDL.h>
#include <vector>
#include "gbcomponent.hpp"

class GBEMU;

class GBDSK : public GBComponent {
public:
    explicit GBDSK(GBEMU* emu);
    void setRom(const std::vector<Uint8>& rom);
    Uint8 readRom(Uint16 address) const;

private:
    std::vector<Uint8> rom;
};

#endif /* GBDSK_HPP */