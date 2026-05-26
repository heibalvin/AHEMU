#include "CH8COM.hpp"
#include "CH8EMU.hpp" // Needs to see CH8EMU to talk to the shared bus matrix

void CH8COM::writeBus(uint16_t address, uint8_t value) {
    emu->bus.write(address, value); // Or your exact internal bus reference layout
}

uint8_t CH8COM::readBus(uint16_t address) const {
    return emu->bus.read(address);
}