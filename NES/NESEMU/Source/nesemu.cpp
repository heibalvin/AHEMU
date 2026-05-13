#include "nesemu.hpp"
#include <cstdio>

NESEMU::NESEMU()
    : masterClock(21477272.0),
      clock(masterClock / 4.0),
      previousTime(0.0),
      deltaTime(0.0),
      cycleId(0),
      ppu(*this) {}

void NESEMU::step() {
    ppu.step();
}

void NESEMU::update(double currentTime) {
    if (previousTime == 0.0) {
        previousTime = currentTime;
        return;
    }

    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    int cyclesToRun = int(deltaTime * clock)
    for(int i = 0; i < cyclesToRun; i++) {
        step();
    }

    cycleId += cyclesToRun;
    printf("Ran %d cycles in %f ms", cyclesToRun, deltaTime);
}

bool NESEMU::isRefreshRequested() const {
    return ppu.isRefreshRequested();
}

void NESEMU::clearRefreshRequest() {
    ppu.clearRefreshRequest();
}

const std::vector<uint8_t>& NESEMU::getFrameBuffer() const {
    return ppu.getFrameBuffer();
}
