#ifndef NESDSK_HPP
#define NESDSK_HPP

#include <SDL3/SDL.h>
#include "nescomponent.hpp"

class NESEMU;

class NESDSK : public NESComponent {
public:
    explicit NESDSK(NESEMU* emu);
    ~NESDSK();
    void debug();

    void loadRom(Uint8* gameRom);

    Uint8* CHR2RGBA(int chrRomIndex, int offset = 0);

private:
    friend class NESEMU;
    friend class NESBUS;
    friend class SDLEMU;
    friend class NESPPU;

    // ROM header information
    int mapper = 0;
    int version = 0;
    int region = 0;     // 0 for NTSC, 1 for PAL
    int mirroring = 0;  // 0 for horizontal, 1 for vertical, 2 for four-screen

    // TRN ROM management (if present)
    int trnRomSize = 0;
    int trnRomCount = 0;
    Uint8 *trnRom = NULL;

    // PRG ROM management
    int prgRomSizeKB = 0;
    int prgRomCount = 0;
    Uint8 **prgRoms = NULL; // Pointers to PRG ROM banks
    int prgRomActive[2] = { 0, 0 }; // Active PRG ROM bank indices for CPU read (if multiple banks are present)

    // CHR ROM management
    int chrRomSizeKB = 0;
    int chrRomCount = 0;
    Uint8 **chrRoms = NULL; // Pointers to CHR ROM banks

    // PRG RAM management  (if present)
    int prgRamSizeKB = 0;
    int prgRamCount = 0;
    Uint8 **prgRams = NULL; // Pointers to PRG RAM banks
};

#endif /* NESDSK_HPP */