#ifndef NESEMU_HPP
#define NESEMU_HPP

#include <SDL3/SDL.h>
#include "nescomponent.hpp"

class NESCPU;
class NESPPU;
class NESDSK;
class NESBUS;

class NESEMU : public NESComponent {
public:
    NESEMU();
    ~NESEMU() override;

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
    const Uint8 *getFrameBuffer() const;

private:
    const Uint64 clock = 3686400; // PPU step rate for 256×240@60fps (3,686,400 steps/sec)
    const Uint64 SDL_NS_TO_SECONDS = 1000000000; // 1 second in nanoseconds
    const Uint64 CYCLE_NS = SDL_NS_TO_SECONDS / clock; // Nanoseconds per cycle
    Uint64 time = 0;
    Uint64 cycleId = 0;

    friend class NESBUS;
    friend class NESCPU;
    friend class NESPPU;
    friend class NESDSK;
    NESBUS* bus;
    NESCPU* cpu;
    NESPPU* ppu;
    NESDSK* dsk;
};

#endif /* NESEMU_HPP */