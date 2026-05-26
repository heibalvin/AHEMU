#include "CH8EMU.hpp"

CH8EMU::CH8EMU() : 
    CH8COM(nullptr), bus(this), cpu(this), ppu(this), con(this), apu(this), dsk(this),
    fpsTarget(60.0), upsTarget(700.0), frameAccumulator(0.0), updateAccumulator(0.0),
    hardwareTimerAccumulator(0.0), profileTimer(0.0), fpsCount(0), upsCount(0),
    fpsCalculated(0), upsCalculated(0), frameReadyFlag(false)
{}

void CH8EMU::powerOn() {
    bus.powerOn();
    ppu.powerOn();
    con.powerOn();
    apu.powerOn();
    dsk.powerOn();
    cpu.powerOn();

    frameAccumulator = 0.0;
    updateAccumulator = 0.0;
    hardwareTimerAccumulator = 0.0;
    profileTimer = 0.0;
    fpsCount = 0;
    upsCount = 0;
    fpsCalculated = 0;
    upsCalculated = 0;
    frameReadyFlag = false;

    // Direct Boot flashing routine querying low-dependency raw pointers from CH8DSK
    if (dsk.hasRom()) {
        const Uint16 programStartOffset = 0x200;
        const Uint16 maxAvailableMemory = 4096;

        if (dsk.getRomSize() <= (maxAvailableMemory - programStartOffset)) {
            SDL_memcpy(&bus.RAM[programStartOffset], dsk.getRomData(), dsk.getRomSize());
            cpu.reset(); // Point program counter back down to 0x200 safely
        } else {
            SDL_Log("Boot Fault: Mounted binary size exceeds standard 4KB system architecture limits.");
        }
    }
}

void CH8EMU::powerOff() {
    cpu.powerOff();
    dsk.powerOff();
    apu.powerOff();
    con.powerOff();
    ppu.powerOff();
    bus.powerOff();
}

void CH8EMU::reset() {
    bus.reset();
    ppu.reset();
    con.reset();
    apu.reset();
    dsk.reset();
    cpu.reset();
}

void CH8EMU::step() {
    cpu.step();
}

void CH8EMU::update(double deltaTime) {
    const double timePerFrame  = 1.0 / fpsTarget;
    const double timePerUpdate = 1.0 / upsTarget;
    const double timePerTimer  = 1.0 / 60.0;

    frameAccumulator         += deltaTime;
    updateAccumulator        += deltaTime;
    hardwareTimerAccumulator += deltaTime;
    profileTimer             += deltaTime;

    while (updateAccumulator >= timePerUpdate) {
        step();
        upsCount++;
        updateAccumulator -= timePerUpdate;
    }

    while (hardwareTimerAccumulator >= timePerTimer) {
        if (cpu.DELAY_TIMER > 0) cpu.DELAY_TIMER--;
        if (cpu.SOUND_TIMER > 0) {
            cpu.SOUND_TIMER--;
            if (cpu.SOUND_TIMER == 0) apu.stop(); //
        }
        hardwareTimerAccumulator -= timePerTimer;
    }

    if (frameAccumulator >= timePerFrame) {
        frameReadyFlag = true;
        fpsCount++;
        frameAccumulator -= timePerFrame;
    }

    if (profileTimer >= 1.0) {
        fpsCalculated = fpsCount;
        upsCalculated = upsCount;
        fpsCount = 0;
        upsCount = 0;
        profileTimer -= 1.0;
    }
}