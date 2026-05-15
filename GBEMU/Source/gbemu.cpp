#include "gbemu.hpp"
#include "gbcpu.hpp"
#include "gbppu.hpp"
#include "gbdsk.hpp"
#include "gbbus.hpp"

GBEMU::GBEMU() : GBComponent(nullptr), bus(nullptr), cpu(nullptr), ppu(nullptr), dsk(nullptr) {
    cpu = new GBCPU(this);
    ppu = new GBPPU(this);
    dsk = new GBDSK(this);
    bus = new GBBUS(this);
}

GBEMU::~GBEMU() {
    delete cpu;
    delete ppu;
    delete dsk;
    delete bus;
}

const int GBEMU::getWidth() {
    return ppu->width;
}

const int GBEMU::getHeight() {
    return ppu->height;
}

void GBEMU::loadRom(Uint8* romData, size_t romSize) {
    // Implementation for loading ROM
    dsk->loadRom(romData, romSize);
    dsk->debug();
}

void GBEMU::start() {
    // Start emulation
}

void GBEMU::pause() {
    // Pause emulation
}

void GBEMU::stop() {
    // Stop emulation and cleanup
}

void GBEMU::update(Uint64 deltaTime) {
    time += deltaTime;
    if (time >= CYCLE_NS) {
        Uint64 cyclesToRun = time / CYCLE_NS;
        time -= cyclesToRun * CYCLE_NS;

        SDL_Log("GBEMU: Running %llu cycles (deltaTime: %llu ns)", cyclesToRun, deltaTime);
        for (Uint64 i = 0; i < cyclesToRun; i++) {
            step();
        }
    }
}

void GBEMU::step() {
    if (cycleId % 4 == 0)
        cpu->step();

    ppu->step();
    cycleId++;
}

bool GBEMU::isRefreshRequested() const {
    return ppu->isRefreshRequested;
}

void GBEMU::clearRefreshRequest() {
    ppu->isRefreshRequested = false;
}

const Uint8 *GBEMU::getFrameBuffer() {
    return ppu->getFrameBuffer();
}