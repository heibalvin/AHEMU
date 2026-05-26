#include "CH8EMU.hpp"

CH8EMU::CH8EMU() : 
    CH8COM(nullptr),
    bus(this), 
    cpu(this), 
    ppu(this), 
    con(this), 
    apu(this),
    dsk(this)
{
    cpuAccumulator   = 0.0;
    timerAccumulator = 0.0;
}

void CH8EMU::powerOn() {
    bus.powerOn();
    ppu.powerOn();
    con.powerOn();
    apu.powerOn();
    dsk.powerOn();
    cpu.powerOn();

    cpuAccumulator   = 0.0;
    timerAccumulator = 0.0;
}

void CH8EMU::powerOff() {
    cpu.powerOff();
    dsk.powerOff();
    apu.powerOff();
    con.powerOff();
    ppu.powerOff();
    bus.powerOff();
}

void CH8EMU::reset() {
    bus.reset();
    ppu.reset();
    con.reset();
    apu.reset();
    dsk.reset();
    cpu.reset();
}

void CH8EMU::step() {
    cpu.step();
}

void CH8EMU::update(double deltaTime) {
    cpuAccumulator   += deltaTime;
    timerAccumulator += deltaTime;

    while (cpuAccumulator >= CPU_PERIOD) {
        step();
        cpuAccumulator -= CPU_PERIOD;
    }

    // 60 Hz Hardware Register Refresh Clock
    while (timerAccumulator >= TIMER_PERIOD) {
        if (cpu.DELAY_TIMER > 0) {
            cpu.DELAY_TIMER--;
        }
        
        if (cpu.SOUND_TIMER > 0) {
            cpu.SOUND_TIMER--;
        }
        
        timerAccumulator -= TIMER_PERIOD;
    }
}

bool CH8EMU::insertRom(const Uint8* data, size_t size) {
    const Uint16 programStartOffset = 0x200;
    const Uint16 maxAvailableMemory = 4096;

    if (size > (maxAvailableMemory - programStartOffset)) {
        return false;
    }

    bus.clearRAM();
    dsk.loadROM((void*)data, size);
    
    SDL_memcpy(&bus.RAM[programStartOffset], dsk.ROM_DATA, dsk.ROM_SIZE);
    
    cpu.reset();
    return true;
}

Uint8 CH8COM::readBus(Uint16 address) const {
    return emu->bus.read(address);
}

void CH8COM::writeBus(Uint16 address, Uint8 value) {
    emu->bus.write(address, value);
}