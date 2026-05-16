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

void NESEMU::loadRom(Uint8* romData, size_t romSize) {
    // Implementation for loading ROM
    dsk->loadRom(romData, romSize);
    dsk->debug();
}

void NESEMU::start() {
    // Start emulation
}

void NESEMU::pause() {
    // Pause emulation
}

void NESEMU::stop() {
    // Stop emulation and cleanup
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
    if (cycleId % 4 == 0)
        cpu->step();

    ppu->step();
    cycleId++;
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