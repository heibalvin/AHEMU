#ifndef GBEMU_HPP
#define GBEMU_HPP

#include <SDL3/SDL.h>
#include <vector>
#include "gbcomponent.hpp"

class GBCPU;
class GBPPU;
class GBDSK;
class GBBUS;

class GBEMU : public GBComponent {
public:
    GBEMU();
    ~GBEMU() override;

    void start();
    void pause();
    void stop();
    void update(Uint64 deltaTime);
    void step(Uint64 deltaTime) override;

    bool isRefreshRequested() const;
    void clearRefreshRequest();
    const std::vector<Uint8>& getFrameBuffer() const;

    // Bus for memory access
    GBBUS* bus;

private:
    friend class GBPPU;
    friend class GBBUS;

    Uint64 clock;
    Uint64 previousTime = 0.0;
    Uint64 deltaTime = 0.0;
    Uint64 cycleId = 0;

    GBCPU* cpu;
    GBPPU* ppu;
    GBDSK* dsk;
};

#endif /* GBEMU_HPP */