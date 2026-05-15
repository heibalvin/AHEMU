#ifndef GBEMU_HPP
#define GBEMU_HPP

#include <SDL3/SDL.h>
#include "gbcomponent.hpp"

class GBCPU;
class GBPPU;
class GBDSK;
class GBBUS;

class GBEMU : public GBComponent {
public:
    GBEMU();
    ~GBEMU() override;

    void loadRom(Uint8* romData, size_t romSize);
    void start();
    void pause();
    void stop();

    void update(Uint64 deltaTime);
    void step() override;

    // sub-component helper functions
    const int getWidth();
    const int getHeight();
    bool isRefreshRequested() const;
    void clearRefreshRequest();
    const Uint8 *getFrameBuffer();

private:
    const Uint64 clock = 4194304.0; // 4.194304 MHz for GB
    const Uint64 SDL_NS_TO_SECONDS = 1000000000; // 1 second in nanoseconds
    const Uint64 CYCLE_NS = SDL_NS_TO_SECONDS / clock; // Nanoseconds per cycle
    Uint64 time = 0;
    Uint64 cycleId = 0;

    friend class GBBUS;
    friend class GBCPU;
    friend class GBPPU;
    friend class GBDSK;
    GBBUS* bus;
    GBCPU* cpu;
    GBPPU* ppu;
    GBDSK* dsk;
};

#endif /* GBEMU_HPP */