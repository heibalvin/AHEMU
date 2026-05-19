#include "nesemu.hpp"
#include "nescpu.hpp"
#include "nesppu.hpp"
#include "nesdsk.hpp"
#include "nesbus.hpp"

NESEMU::NESEMU() : NESComponent(nullptr), bus(nullptr), cpu(nullptr), ppu(nullptr), dsk(nullptr) {
    cpu = new NESCPU(this);
    ppu = new NESPPU(this);
    dsk = new NESDSK(this);
    bus = new NESBUS(this);
}

NESEMU::~NESEMU() {
    delete cpu;
    delete ppu;
    delete dsk;
    delete bus;
}

const int NESEMU::getWidth() {
    return ppu->width;
}

const int NESEMU::getHeight() {
    return ppu->height;
}

void NESEMU::loadRom(Uint8* romData) {
    // Implementation for loading ROM
    dsk->loadRom(romData);
    dsk->debug();
}

void NESEMU::powerOn() {
    // Start emulation
    ppu->powerOn();
    cpu->powerOn();
}

void NESEMU::update(Uint64 deltaTime) {
    time += deltaTime;
    if (time >= CYCLE_NS) {
        Uint64 cyclesToRun = time / CYCLE_NS;
        time -= cyclesToRun * CYCLE_NS;

        // SDL_Log("NESEMU: Running %llu cycles (deltaTime: %llu ns)", cyclesToRun, deltaTime);
        for (Uint64 i = 0; i < cyclesToRun; i++) {
            step();
        }
    }
}

void NESEMU::step() {
    // 1. Advance the CPU by one system step clock cycle
    cpu->step();

    // 2. The PPU runs exactly 3 times faster than the CPU master clock sequence
    ppu->step();
    ppu->step();
    ppu->step();
    
    // 3. Handle additional peripheral timing intervals (like APU or Mapper counters)
}

bool NESEMU::isRefreshRequested() const {
    return ppu->isRefreshRequested;
}

void NESEMU::clearRefreshRequest() {
    ppu->isRefreshRequested = false;
}

const Uint8 *NESEMU::getFrameBuffer() const {
    return ppu->getFrameBuffer();
}

void NESEMU::raiseEvent(NESEvent event) {
    // If this event matches our current debugging focus, halt the entire motherboard instantly!
    if (event == haltTarget || event == NESEvent::FRAME_COMPLETE) {
        isHalted = true;
        lastTriggeredEvent = event;
    }
}

void NESEMU::runUntilEvent() {
    resume(); // Clear old event states and arm the engine

    while (true) {
        // Exit: halted and NOT in continuous-run mode (step-by-step stop)
        if (isHalted && !isContinuousRun) break;

        // Auto-resume: keep the pipeline flowing in Run mode
        if (isHalted && isContinuousRun) resume();

        // ── 1 CPU master cycle ──────────────────────────────────
        cpu->step();

        // ── 3 PPU ticks per CPU cycle (1:3 ratio) ──────────────
        ppu->step();
        ppu->step();
        ppu->step();

        totalMasterCycles++;
    }
}