#include "CH8APU.hpp"
#include "CH8EMU.hpp" // Ensure you include the parent matrix to resolve cpu fields

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
    // No longer strictly necessary if reading directly from timers
    buzzerActive = true;
}

void CH8APU::stop() {
    buzzerActive = false;
}

bool CH8APU::isBuzzerActive() const {
    // The buzzer is active as long as the CPU's hardware sound timer register holds electricity
    return emu->cpu.SOUND_TIMER > 0;
}