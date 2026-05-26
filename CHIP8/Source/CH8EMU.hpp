#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"
#include "CH8BUS.hpp"
#include "CH8CPU.hpp"
#include "CH8PPU.hpp"
#include "CH8CON.hpp"
#include "CH8APU.hpp"
#include "CH8DSK.hpp"

class CH8EMU : public CH8COM {
public:
    // --- Central Subsystem Core Matrix ---
    CH8BUS bus;
    CH8CPU cpu;
    CH8PPU ppu;
    CH8CON con;
    CH8APU apu;
    CH8DSK dsk;

private:
    double cpuAccumulator;
    double timerAccumulator;

    const double CPU_PERIOD   = 1.0 / 500.0; // 500Hz Instruction ticking rate
    const double TIMER_PERIOD = 1.0 / 60.0;  // 60Hz Hardware countdown refresh line

public:
    CH8EMU();
    virtual ~CH8EMU() = default;

    void powerOn() override;
    void powerOff() override;
    void reset() override;
    void step() override;

    void update(double deltaTime);
    bool insertRom(const Uint8* data, size_t size);
};