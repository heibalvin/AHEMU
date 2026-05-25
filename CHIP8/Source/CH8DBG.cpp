#if CH8_DEBUG_ENABLED

#include "CH8DBG.hpp"
#include "CH8COM.hpp"
#include "CH8EMU.hpp"

CH8DBG::CH8DBG(CH8EMU* parentEmu) : CH8COM(parentEmu) {
    reset();
}

void CH8DBG::powerOn() {
    reset();
}

void CH8DBG::powerOff() {
    SDL_memset(breakpointsMatrix, 0, sizeof(breakpointsMatrix));
}

void CH8DBG::reset() {
    currentState = DebugState::RUNNING;
    SDL_memset(breakpointsMatrix, 0, sizeof(breakpointsMatrix));
}

bool CH8DBG::checkLifecycle(Uint16 currentPC) {
    // If we hit an address flagged in our breakpoint lookup map, halt execution
    if (currentPC < 4096 && breakpointsMatrix[currentPC]) {
        currentState = DebugState::PAUSED;
        SDL_Log("Debugger Trapped Breakpoint at Memory Reference Location: 0x%04X", currentPC);
    }

    if (currentState == DebugState::STEPPING) {
        currentState = DebugState::PAUSED;
        return true; 
    }

    return (currentState == DebugState::RUNNING);
}

void CH8DBG::toggleBreakpoint(Uint16 address) {
    if (address < 4096) {
        breakpointsMatrix[address] = !breakpointsMatrix[address];
    }
}

void CH8DBG::pauseExecution() {
    currentState = DebugState::PAUSED;
}

void CH8DBG::resumeExecution() {
    currentState = DebugState::RUNNING;
}

void CH8DBG::stepInstruction() {
    currentState = DebugState::STEPPING;
}

DebugState CH8DBG::getState() const {
    return currentState;
}

#endif