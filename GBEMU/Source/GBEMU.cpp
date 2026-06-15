#include "GBEMU.h"

// 1. Use an initializer list to construct components with 'this'
GBEMU::GBEMU() 
    : isRunning(false), bus(this), dsk(this), cpu(this), vdp(this), joy(this), apu(this) {
        
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

int GBEMU::step() {
    // Implement your main emulation cycle here
    return 0;
}

void GBEMU::load(const Uint8* data, size_t size) {
    dsk.load(data, size);
}