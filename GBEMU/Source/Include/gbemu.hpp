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

    void update(double deltaTime);
    void step() override;

    bool isRefreshRequested() const;
    void clearRefreshRequest();
    const Uint8 *getFrameBuffer();

private:
    static constexpr double GB_CLOCK_HZ = 4194304.0;
    static constexpr double CYCLE_NS = 1000000000.0 / GB_CLOCK_HZ;
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