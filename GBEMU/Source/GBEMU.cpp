#include "GBEMU.h"

// 1. Use an initializer list to construct components with 'this'
GBEMU::GBEMU() 
    : isRunning(false), bus(*this), dsk(*this), cpu(*this), vdp(*this), joy(*this), apu(*this) {
        
    }

GBEMU::~GBEMU() {

}

bool GBEMU::powerOn() {
    SDL_Log("EMU: powerOn()");
    
    // 2. Use dot operator (.), not arrow (->)
    bus.powerOn();
    dsk.powerOn();
    
    isRunning = true;
    return true;
}

void GBEMU::powerOff() {
    SDL_Log("EMU: powerOff()");
    dsk.powerOff();
    bus.powerOff();
    isRunning = false;
}

void GBEMU::reset() {
	SDL_Log("EMU: reset()");
    bus.reset();
    dsk.reset();
    isRunning = true;
}

void GBEMU::step() {
    int cpuCycles = cpu.step(); // Ensure step() returns cycles consumed
    vdp.step(cpuCycles);
    // apu.step(cpuCycles); // Future-proofing
}

void GBEMU::run(Uint64 elapsedTicks) {
    // Convert elapsed nanoseconds to cycles (GB Master Clock: ~4.19 MHz)
    // 4,194,304 cycles / 1,000,000,000 ns
    static const double cyclesPerNs = 4.194304; 
    int cyclesToRun = (int)(elapsedTicks * cyclesPerNs);
    
    // Prevent "spiral of death" after long pauses
    if (cyclesToRun > 70224) cyclesToRun = 70224; // Limit to ~1 frame worth of cycles

    while (cyclesToRun > 0) {
        // Execute CPU and keep components aligned
        int cpuCycles = cpu.step(); // Ensure step() returns cycles consumed
        vdp.step(cpuCycles);
        // apu.step(cpuCycles); // Future-proofing
        
        cyclesToRun -= cpuCycles;
    }
}

void GBEMU::load(const Uint8* data, size_t size) {
    dsk.load(data, size);
}

void GBEMU::setJoypadButton(int button, bool pressed) {
    joy.setButton(static_cast<GBJOY::Button>(button), pressed);
}

// VDP render helper
const Uint32* GBEMU::getVDPFrameBuffer() const {
    return vdp.getFrontBuffer();
}

void GBEMU::updateIRQRequest() {
    // An interrupt is requested if a bit is set in IF AND also enabled in IE
    isIRQRequested = (IF & IE) != 0;
}