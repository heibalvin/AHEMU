#include "gbdsk.hpp"

GBDSK::GBDSK(GBEMU* emu) : GBComponent(emu) {}

void GBDSK::setRom(const std::vector<Uint8>& rom) {
    this->rom = rom;
}

Uint8 GBDSK::readRom(Uint16 address) const {
    if (address < rom.size()) {
        return rom[address];
    }
    return 0xFF; // Return 0xFF for out of bounds (typical for unmapped memory)
}
