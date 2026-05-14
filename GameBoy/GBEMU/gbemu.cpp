#include "gbemu.hpp"
#include "gbcpu.hpp"
#include "gbppu.hpp"
#include "gbdsk.hpp"

GBEMU::GBEMU() : cpu(nullptr), ppu(nullptr), dsk(nullptr) {
    clock = 1000 / 4.194304;            // 238.4 nanosec
    cpu = new GBCPU(this);
    ppu = new GBPPU(this);
    dsk = new GBDSK(this);
}

GBEMU::~GBEMU() {
    delete cpu;
    delete ppu;
    delete dsk;
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
    int steps = static_cast<int>(deltaTime * clock);
    for (int i = 0; i < steps; i++) {
        step();
    }
}

void GBEMU::step() {
    if (cycleId % 4 == 0)
        cpu->step();

    ppu->step();
}

bool GBEMU::isRefreshRequested() const {
    return ppu->refreshRequested;
}

void GBEMU::clearRefreshRequest() {
    ppu->refreshRequested = false;
}

const std::vector<uint8_t>& GBEMU::getFrameBuffer() const {
    return ppu->getFrameBuffer();
}