#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8DSK : public CH8COM {
public:
    // Raw binary buffer allocations mirroring original media sectors
    Uint8* ROM_DATA;
    size_t ROM_SIZE;

public:
    CH8DSK(CH8EMU* parentEmu);
    virtual ~CH8DSK();

    // --- Core Component Lifecycle Protocol Contracts ---
    void powerOn() override;
    void powerOff() override;
    void reset() override;

    /**
     * Allocates standard system memory sectors and copies raw payload configurations.
     * @param data Pointer to the source binary segment payload buffer block.
     * @param size Total length in bytes of the target application payload data.
     */
    bool loadROM(const void* data, size_t size);

    /**
     * Explicit clean routine to flush storage blocks and clear memory leaking.
     */
    void clearStorage();
};