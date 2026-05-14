#ifndef GBEMU_HPP
#define GBEMU_HPP

#include <SDL3/SDL.h>
#include <vector>

class GBCPU;
class GBPPU;
class GBDSK;

class GBEMU {
public:
    GBEMU();
    ~GBEMU();

    void start();
    void pause();
    void stop();
    void update(Uint64 deltaTime);
    void step();

    bool isRefreshRequested() const;
    void clearRefreshRequest();
    const std::vector<Uint8>& getFrameBuffer() const;

private:
    friend class GBPPU;

    Uint64 clock;
    Uint64 previousTime = 0.0;
    Uint64 deltaTime = 0.0;
    Uint64 cycleId = 0;

    GBCPU* cpu;
    GBPPU* ppu;
    GBDSK* dsk;
};

#endif /* GBEMU_HPP */