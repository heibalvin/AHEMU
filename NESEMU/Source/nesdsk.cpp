#include "nesdsk.hpp"

NESDSK::NESDSK(NESEMU* emu) : NESComponent(emu) {}

NESDSK::~NESDSK() {
    if (gamerom) {
        SDL_free(gamerom);
        gamerom = NULL;
    }
    if (trnRoms) {
        SDL_free(trnRoms);
        trnRoms = NULL;
    }
    if (prgRoms) {
        SDL_free(prgRoms);
        prgRoms = NULL;
    }
    if (chrRoms) {
        SDL_free(chrRoms);
        chrRoms = NULL;
    }
    if (prgRams) {
        SDL_free(prgRams);
        prgRams = NULL;
    }
}

void NESDSK::debug() {
    SDL_Log("NESDSK: Mapper: %d", mapper);
    SDL_Log("\tVersion: %d", version);
    SDL_Log("\tRegion: %s", region == 0 ? "NTSC" : "PAL");
    SDL_Log("\tMirroring: %s", mirroring == 0 ? "Horizontal" : (mirroring == 1 ? "Vertical" : "Four-screen"));
    SDL_Log("\tTRN ROM: %d KB * %d count", trnRomSize / 1024, trnRomCount);
    SDL_Log("\tPRG ROM: %d KB * %d count", prgRomSizeKB, prgRomCount);
    SDL_Log("\tCHR ROM: %d KB * %d count", chrRomSizeKB, chrRomCount);
    SDL_Log("\tPRG RAM: %d KB * %d count", prgRamSizeKB, prgRamCount);
}

void NESDSK::loadRom(Uint8* romData, size_t romSize) {
    this->gamerom = romData;
    this->romSize = romSize;

    decode();
}

void NESDSK::decode() {
    // Implementation for parsing ROM data and extracting information such as title, cartridge type, ROM size, RAM size, etc.

    if (gamerom[0x00] != 'N' || gamerom[0x01] != 'E' || gamerom[0x02] != 'S' || gamerom[0x03] != 0x1A) {
        SDL_Log("NESDSK: Invalid ROM header");
        return;
    }

    // Extract mapper number (1 byte at 0x0F)
    mapper = (gamerom[0x07] & 0xF0) | (gamerom[0x06] >> 4);

    // Extract compatibility flags
    version = (gamerom[0x07] & 0x0F) >> 2 == 2 ? 2 : 1; // NES 2.0 if lower 2 bits of byte 7 are 10

    // Extract TV region standard
    region = gamerom[0x08] & 0x01; // 0 for NTSC, 1 for PAL

    // Extract Mirroring type
    mirroring = (gamerom[0x06] & 0x08 >> 2) | (gamerom[0x06] & 0x01); // 0 for horizontal, 1 for vertical, 2 for four-screen

    // Start of ROM data after header
    Uint16 addr = 0x10;

    // Extract TRN RAM information (if present)
    if ((gamerom[0x06] & 0x04) != 0) {
        trnRomSize = 512;
        trnRomCount = 1;
        trnRoms = (Uint8 **)SDL_malloc(sizeof(Uint8*) * trnRomCount);
        for(int i = 0; i < trnRomCount; i++) {
            // Setup TRN ROM banking pointers
            trnRoms[i] = &gamerom[addr];
            addr += trnRomSize;
        }
    }

    // Extract PRG ROM information
    prgRomSizeKB = 16;                          // PRG ROM size in KB
    prgRomCount = gamerom[0x04];                // PRG ROM count in 16KB units
    prgRoms = (Uint8 **)SDL_malloc(sizeof(Uint8*) * prgRomCount);
    for(int i = 0; i < prgRomCount; i++) {
        // Setup PRG ROM banking pointers
        prgRoms[i] = &gamerom[addr];
        addr += prgRomSizeKB * 1024;
    }
    prgRomActive[0] = 0;                        // Start with first PRG ROM bank active for CPU read
    prgRomActive[1] = prgRomCount - 1;          // Last PRG ROM bank is often fixed at 0xC000-0xFFFF

    // Extract CHR ROM information
    chrRomSizeKB = 8;                           // CHR ROM size in KB
    chrRomCount = gamerom[0x05];                // CHR ROM count in 8KB units
    chrRoms = (Uint8 **)SDL_malloc(sizeof(Uint8*) * chrRomCount);
    for(int i = 0; i < chrRomCount; i++) {
        // Setup CHR ROM banking pointers
        chrRoms[i] = &gamerom[addr];
        addr += chrRomSizeKB * 1024;
    }

    // Extract PRG RAM information (if present)
    if ((gamerom[0x06] & 0x02) != 0) {
        prgRamSizeKB = 8;                           // PRG RAM size in KB
        prgRamCount = gamerom[0x08];                // PRG RAM count in 8KB units
        prgRams = (Uint8 **)SDL_malloc(sizeof(Uint8*) * prgRamCount);
        for(int i = 0; i < prgRamCount; i++) {
            // Setup PRG RAM banking pointers
            prgRams[i] = &gamerom[addr];
            addr += prgRamSizeKB * 1024;
        }
    }
}