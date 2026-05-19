#ifndef NESEMU_HPP
#define NESEMU_HPP

#include <SDL3/SDL.h>
#include "nescomponent.hpp"

class NESCPU;
class NESPPU;
class NESDSK;
class NESBUS;

enum class NESEvent {
    NONE,
    CYCLE_STEP,        // A single clock cycle ticked
    INSTRUCTION_STEP,  // An entire instruction finished execution
    VBLANK_START,      // PPU entered scanline 241
    FRAME_COMPLETE,    // PPU finished scanline 261
    NMI_TRIGGERED,     // NMI Interrupt Vector was entered
    IRQ_TRIGGERED      // IRQ Interrupt Vector was entered
};

class NESEMU : public NESComponent {
public:
    NESEMU();
    ~NESEMU() override;

    void loadRom(Uint8* romData);
    void powerOn() override;

    void update(Uint64 deltaTime);
    void step() override;

    // sub-component helper functions
    const int getWidth();
    const int getHeight();
    bool isRefreshRequested() const;
    void clearRefreshRequest();
    const Uint8 *getFrameBuffer() const;

    // Debugging controls
    void setHaltTarget(NESEvent target) { haltTarget = target; }
    void setContinuousRun(bool enable) { isContinuousRun = enable; }
    void resume() { isHalted = false; lastTriggeredEvent = NESEvent::NONE; }
    bool halted() const { return isHalted; }
    NESEvent getLastEvent() const { return lastTriggeredEvent; }

    // This is the core loop called by SDLEMU
    void runUntilEvent();

    // AUTONOMOUS GATEWAY: Sub-components call this to raise an event
    void raiseEvent(NESEvent event);

private:
    const Uint64 clock = 3686400; // PPU step rate for 256×240@60fps (3,686,400 steps/sec)
    const Uint64 SDL_NS_TO_SECONDS = 1000000000; // 1 second in nanoseconds
    const Uint64 CYCLE_NS = SDL_NS_TO_SECONDS / clock; // Nanoseconds per cycle
    Uint64 time = 0;
    Uint64 cycleId = 0;

    friend class SDLEMU;
    friend class NESBUS;
    friend class NESCPU;
    friend class NESPPU;
    friend class NESDSK;
    NESBUS* bus;
    NESCPU* cpu;
    NESPPU* ppu;
    NESDSK* dsk;

    bool isHalted = true;
    bool isContinuousRun = false;
    NESEvent haltTarget = NESEvent::FRAME_COMPLETE;
    NESEvent lastTriggeredEvent = NESEvent::NONE;
    
    // Core system clock counters
    Uint64 totalMasterCycles = 0;
};

#endif /* NESEMU_HPP */