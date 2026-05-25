#include "CH8EMU.hpp"

// ============================================================================
// CH8EMU HARDWARE INSTANTIATION & LIFECYCLE ROUTINES
// ============================================================================

CH8EMU::CH8EMU() : 
    CH8COM(nullptr), // Master core matrix holds no parent pointer context
    bus(this), 
    cpu(this), 
    ppu(this), 
    con(this), 
    apu(this),
    dsk(this)
    CH8_DBG_INIT(dbg(this)) // Injection macro updates initialization signature lists
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
    CH8_DBG_EXEC(dbg.powerOn());
    cpu.powerOn(); // CPU wakes up last to process safe structural entry spaces

    cpuAccumulator   = 0.0;
    timerAccumulator = 0.0;
}

void CH8EMU::powerOff() {
    cpu.powerOff();
    CH8_DBG_EXEC(dbg.powerOff());
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
    CH8_DBG_EXEC(dbg.reset());
    cpu.reset();
}

void CH8EMU::step() {
    cpu.step();
}

// ============================================================================
// SYNCHRONIZED CENTRAL CLOCK ENGINE PROCESSING LOOPS
// ============================================================================

void CH8EMU::update(double deltaTime) {
    cpuAccumulator   += deltaTime;
    timerAccumulator += deltaTime;

    // 1. Process Virtual CPU Instruction Pipeline Steps
    while (cpuAccumulator >= CPU_PERIOD) {
        bool proceed = true;
        
        // Single-line macro intercepts active instructions smoothly
        CH8_DBG_EXEC(proceed = dbg.checkLifecycle(cpu.PC));
        
        if (proceed) {
            step();
            cpuAccumulator -= CPU_PERIOD;
        } else {
            break; // Breakpoint encountered; freeze clock loops instantly
        }
    }

    // 2. Process Independent Decoupled 60Hz Decrement Hardware Timers
    while (timerAccumulator >= TIMER_PERIOD) {
        if (cpu.DELAY_TIMER > 0) {
            cpu.DELAY_TIMER--;
        }
        
        if (cpu.SOUND_TIMER > 0) {
            cpu.SOUND_TIMER--;
            apu.step(); // Keeps internal headless audio active
        } else {
            apu.stop(); // Cut current channel playback paths instantly
        }
        
        timerAccumulator -= TIMER_PERIOD;
    }
}

bool CH8EMU::injectROM(const Uint8* data, size_t size) {
    const Uint16 programStartOffset = 0x200;
    const Uint16 maxAvailableMemory = 4096;

    if (size > (maxAvailableMemory - programStartOffset)) {
        return false; // Payload constraints violation layout catch
    }

    bus.clearRAM();
    dsk.loadROM((void*)data, size);
    
    // Low-level memory block array replication pass
    SDL_memcpy(&bus.RAM[programStartOffset], dsk.ROM_DATA, dsk.ROM_SIZE);
    
    cpu.reset(); // Snap registers and program counter positions to baseline
    return true;
}

// ============================================================================
// CENTRALIZED DECENTRALIZED BUS PIPELINE ROUTERS
// ============================================================================

Uint8 CH8COM::readBus(Uint16 address) const {
    return emu->bus.read(address);
}

void CH8COM::writeBus(Uint16 address, Uint8 value) {
    emu->bus.write(address, value);
}