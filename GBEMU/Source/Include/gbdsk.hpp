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

    void loadRom(const Uint8* romData, size_t romSize);
    Uint8 readRom(Uint16 address);
    void parseRom(const Uint8* romData, size_t romSize);

private:
    Uint8 *rom;
    size_t romSize;

    char title[16] = "";
    char cartridgeType[128] = "";
    bool isDMGCompatible = false;
    bool isCGBCompatible = false;
    int romSizeKB = 0;
    int romCount = 0;
    int ramSizeKB = 0;
    int ramCount = 0;

    /*
    Uint8 *entryPoint = NULL;
    Uint8 *nintendoLogo = NULL;
    Uint8 *title = NULL;
    Uint8 *manufacturerCode = NULL;
    Uint8 cgbFlag = 0;
    Uint8 *newLicenseeCode = NULL;
    Uint8 sgbFlag = 0;
    Uint8 cartridgeType = 0;
    Uint8 romSizeCode = 0;
    Uint8 ramSizeCode = 0;
    Uint8 destinationCode = 0;
    Uint8 oldLicenseeCode = 0;
    Uint8 maskRomVersion = 0;
    Uint8 headerChecksum = 0;
    Uint16 globalChecksum = 0;
    */
};

#endif /* GBDSK_HPP */