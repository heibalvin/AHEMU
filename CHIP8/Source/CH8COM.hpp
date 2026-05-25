#pragma once
#include <SDL3/SDL.h>

class CH8EMU;

// --- High-Performance Micro-Conditional Macro Subsystem ---
#if CH8_DEBUG_ENABLED
    #define CH8_DBG_EXEC(code)  do { code; } while(0)
    #define CH8_DBG_MEMBER(decl) decl;              
    #define CH8_DBG_INIT(init)   , init             
#else
    #define CH8_DBG_EXEC(code)  do { } while(0)
    #define CH8_DBG_MEMBER(decl)                     
    #define CH8_DBG_INIT(init)                       
#endif

class CH8COM {
protected:
    CH8EMU* emu = nullptr;

public:
    CH8COM() = default;
    CH8COM(CH8EMU* parentEmu) : emu(parentEmu) {}
    virtual ~CH8COM() = default;

    virtual void powerOn() = 0;
    virtual void powerOff() = 0;
    virtual void reset() = 0;
    virtual void step() {}

    // Decentralized motherboard pipeline access wrappers
    Uint8 readBus(Uint16 address) const;
    void writeBus(Uint16 address, Uint8 value);
};