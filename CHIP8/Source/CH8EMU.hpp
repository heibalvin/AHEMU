#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"
#include "CH8BUS.hpp"
#include "CH8CPU.hpp"
#include "CH8PPU.hpp"
#include "CH8CON.hpp"
#include "CH8APU.hpp"
#include "CH8DSK.hpp"
#include "CH8DBG.hpp" // Conditionally stubs itself out inside its own header via macros

class CH8EMU : public CH8COM {
public:
    // --- Central Subsystem Core Matrix ---
    CH8BUS bus;
    CH8CPU cpu;
    CH8PPU ppu;
    CH8CON con;
    CH8APU apu;
    CH8DSK dsk;

    // Macro manages conditional debugger compilation allocation
    CH8_DBG_MEMBER(CH8DBG dbg)

private:
    // Time slicing metrics trackers
    double cpuAccumulator;
    double timerAccumulator;

    // Fixed hardware clock speed parameters
    const double CPU_PERIOD   = 1.0 / 500.0; // 500Hz clock speed execution cycle
    const double TIMER_PERIOD = 1.0 / 60.0;  // 60Hz standard countdown refresh line

public:
    CH8EMU();
    virtual ~CH8EMU() = default;

    // --- Core Component Lifecycle Protocol Contracts ---
    void powerOn() override;
    void powerOff() override;
    void reset() override;
    void step() override;

    /**
     * Ticks the core pipeline, balancing accumulated frame delays 
     * and reducing sound/delay registers at exact intervals.
     * @param deltaTime Fractional seconds since the platform loop frame ran.
     */
    void update(double deltaTime);

    /**
     * Mounts a raw binary data buffer into execution RAM at space 0x200.
     */
    bool injectROM(const Uint8* data, size_t size);
};