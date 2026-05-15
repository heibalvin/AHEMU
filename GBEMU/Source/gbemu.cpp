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

void GBEMU::update(double deltaTimeNs) {
    Uint64 cyclesToRun = static_cast<Uint64>(deltaTimeNs / CYCLE_NS);
    if (cyclesToRun == 0) cyclesToRun = 1; // Always run at least 1 cycle
    for (Uint64 i = 0; i < cyclesToRun; i++) {
        step();
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