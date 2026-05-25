#pragma once
#include <SDL3/SDL.h>

#if CH8_DEBUG_ENABLED

#include "CH8COM.hpp"

enum class DebugState { RUNNING, PAUSED, STEPPING };

class CH8DBG : public CH8COM {
private:
    DebugState currentState;
    bool breakpointsMatrix[4096];

public:
    CH8DBG(CH8EMU* parentEmu);
    virtual ~CH8DBG() = default;

    void powerOn() override;
    void powerOff() override;
    void reset() override;

    bool checkLifecycle(Uint16 currentPC);
    
    void toggleBreakpoint(Uint16 address);
    void pauseExecution();
    void resumeExecution();
    void stepInstruction();
    DebugState getState() const;
};

#else

// Zero-overhead production release layout substitute stub
enum class DebugState { RUNNING };

class CH8DBG {
public:
    CH8DBG() {}
    CH8DBG(void* ignored) {}
    ~CH8DBG() = default;

    void powerOn() {}
    void powerOff() {}
    void reset() {}

    bool checkLifecycle(Uint16 currentPC) { return true; }
    void toggleBreakpoint(Uint16 address) {}
    void pauseExecution() {}
    void resumeExecution() {}
    void stepInstruction() {}
    DebugState getState() const { return DebugState::RUNNING; }
};

#endif