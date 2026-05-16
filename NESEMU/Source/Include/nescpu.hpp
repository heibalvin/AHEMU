#ifndef NESCPU_HPP
#define NESCPU_HPP

#include <SDL3/SDL.h>
#include "nescomponent.hpp"

class NESEMU;

class NESCPU : public NESComponent {
public:
    explicit NESCPU(NESEMU* emu);
    ~NESCPU();

    void powerOn() override;
    void step() override;

    void fetch();
    void decode();
    void execute();

private:
    friend class NESEMU;
    friend class NESBUS;

    // Work RAM (WRAM) management
    Uint8 *wram = NULL; // Work RAM (WRAM) for the main memory
};

#endif /* NESCPU_HPP */