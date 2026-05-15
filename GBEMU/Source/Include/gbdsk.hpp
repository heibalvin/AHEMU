#ifndef GBDSK_HPP
#define GBDSK_HPP

#include <SDL3/SDL.h>
#include "gbcomponent.hpp"

class GBEMU;

class GBDSK : public GBComponent {
public:
    explicit GBDSK(GBEMU* emu);
    ~GBDSK();
    void debug();

    void loadRom(Uint8* romData, size_t romSize);
    void decode();

private:
    friend class GBBUS;

    // ROM data and metadata
    Uint8 *gamerom;  // Keep const for MBC banking
    size_t romSize;

    // ROM header information
    char title[16] = "";
    char cartridgeType[128] = "";
    bool isDMGCompatible = false;
    bool isCGBCompatible = false;
    
    // MBC ROM banking data
    int romSizeKB = 0;
    int romCount = 0;
    Uint8 **roms = NULL;
    int romActive[2] = { 0, 1 };

    // MBC RAM banking data
    int ramSizeKB = 0;
    int ramCount = 0;
    Uint8 **rams = NULL;
    int ramActive = 0;
};

#endif /* GBDSK_HPP */