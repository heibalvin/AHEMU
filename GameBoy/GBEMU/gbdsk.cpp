#include "gbdsk.hpp"
#include "gbemu.hpp"

GBDSK::GBDSK(GBEMU* emu) : emu(emu) {
    (void)this->emu;
}

void GBDSK::step() {
    // Disk/drive step logic
}