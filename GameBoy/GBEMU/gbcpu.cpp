#include "gbcpu.hpp"
#include "gbemu.hpp"

GBCPU::GBCPU(GBEMU* emu) : emu(emu) {
    (void)this->emu;
}

void GBCPU::step() {
    // CPU step logic
}