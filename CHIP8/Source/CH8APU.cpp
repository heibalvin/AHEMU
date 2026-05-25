#include "CH8APU.hpp"

CH8APU::CH8APU(CH8EMU* parentEmu) : CH8COM(parentEmu) {}

void CH8APU::powerOn() {
    buzzerActive = false;
}

void CH8APU::powerOff() {
    buzzerActive = false;
}

void CH8APU::reset() {
    buzzerActive = false;
}

void CH8APU::step() {
    buzzerActive = true;
}

void CH8APU::stop() {
    buzzerActive = false;
}

bool CH8APU::isBuzzerActive() const {
    return buzzerActive;
}