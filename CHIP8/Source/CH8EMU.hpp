#pragma once
#include <SDL3/SDL.h>
#include "CH8BUS.hpp"
#include "CH8CPU.hpp"
#include "CH8PPU.hpp"
#include "CH8CON.hpp"
#include "CH8APU.hpp"
#include "CH8DSK.hpp"
#include "CH8COM.hpp"


class CH8EMU : public CH8COM {
public:
    CH8BUS bus;
    CH8CPU cpu;
    CH8PPU ppu;
    CH8CON con;
    CH8APU apu;
    CH8DSK dsk;

private:
    double fpsTarget;
    double upsTarget;

    double frameAccumulator;
    double updateAccumulator;
    double hardwareTimerAccumulator;
    double profileTimer;

    int fpsCount;
    int upsCount;
    int fpsCalculated;
    int upsCalculated;

    bool frameReadyFlag;

public:
    CH8EMU();
    ~CH8EMU() = default;

    void powerOn();
    void powerOff();
    void reset();
    void step();
    void update(double deltaTime);

    // Dynamic interface pass-through straight to low-dependency DSK
    bool insertRom(const Uint8* datas, size_t size) { return dsk.insertRom(datas, size); }

    void setFpsTarget(double target) { fpsTarget = target; }
    void setUpsTarget(double target) { upsTarget = target; }
    double getFpsTarget() const { return fpsTarget; }
    double getUpsTarget() const { return upsTarget; }
    int getFpsCalculated() const { return fpsCalculated; }
    int getUpsCalculated() const { return upsCalculated; }

    bool isFrameReady() const { return frameReadyFlag; }
    void clearFrameReadyFlag() { frameReadyFlag = false; }
};