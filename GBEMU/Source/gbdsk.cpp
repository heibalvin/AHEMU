#include "gbdsk.hpp"

GBDSK::GBDSK(GBEMU* emu) : GBComponent(emu) {}

GBDSK::~GBDSK() {
    if (roms) {
        SDL_free(roms);
        roms = nullptr;
    }
    if (rams) {
        SDL_free(rams);
        rams = nullptr;
    }
}

void GBDSK::debug() {
    SDL_Log("GBDSK: Title: %s", title);
    SDL_Log("\tCartridge type: %s", cartridgeType);
    SDL_Log("\tDMG compatible: %s", isDMGCompatible ? "Yes" : "No");
    SDL_Log("\tCGB compatible: %s", isCGBCompatible ? "Yes" : "No");
    SDL_Log("\tROM size: %d KB * %d count", romSizeKB, romCount);
    SDL_Log("\tRAM size: %d KB * %d count", ramSizeKB, ramCount);
}

void GBDSK::loadRom(Uint8* romData, size_t romSize) {
    this->gamerom = romData;
    this->romSize = romSize;

    decode();
}

void GBDSK::decode() {
    // Implementation for parsing ROM data and extracting information such as title, cartridge type, ROM size, RAM size, etc.

    // 0134-0143 — Title (padded with 0)
    SDL_strlcpy(title, (const char*)(gamerom + 0x0134), sizeof(title));

    // 0143 — CGB flag
    Uint8 cgbFlag = gamerom[0x0143];
    if (cgbFlag == 0x80) {
        isCGBCompatible = true;
        isDMGCompatible = true;
    } else if (cgbFlag == 0xC0) {
        isCGBCompatible = true;
        isDMGCompatible = false;
    } else {
        isCGBCompatible = false;
        isDMGCompatible = true;
    }
    
    // 0147 — Cartridge type
    const char *cartridgeTypes[] = {
        "ROM ONLY",
        "MBC1",
        "MBC1+RAM",
        "MBC1+RAM+BATTERY",
        "MBC2",
        "MBC2+BATTERY",
        "ROM+RAM",
        "ROM+RAM+BATTERY",
        "MMM01",
        "MMM01+RAM",
        "MMM01+RAM+BATTERY",
        "MBC3+TIMER+BATTERY",
        "MMC3",
        "MMC3+RAM",
        "MMC3+RAM+BATTERY",
        "MBC5",
        "MBC5+RAM",
        "MBC5+RAM+BATTERY",
        "MBC5+RUMBLE",
        "MBC5+RUMBLE+RAM",
        "MBC5+RUMBLE+RAM+BATTERY",
        "POCKET CAMERA",
        "BANDAI TAMA5",
        "HuC3",
        "HuC1",
    };

    const Uint8 cartridgeIndexes[] = {
        0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x08, 0x09, 0x0B, 0x0C, 0x0D,
        0x0F, 0x10, 0x11, 0x12, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
        0x20, 0x22, 0xFC, 0xFD
    };
    Uint8 cartridgeTypeCode = gamerom[0x0147];
    SDL_strlcpy(cartridgeType, cartridgeTypes[cartridgeIndexes[cartridgeTypeCode]], sizeof(cartridgeType));

    // 0148 — ROM size
    romSizeKB = 16;
    if (gamerom[0x0148] <= 0x08) {
        romCount = 2 * (1 << gamerom[0x0148]);
    } else if (gamerom[0x0148] == 0x52) {
        romCount = 72;
    } else if (gamerom[0x0148] == 0x53) {
        romCount = 80;
    } else if (gamerom[0x0148] == 0x54) {
        romCount = 96;
    }

    Uint16 addr = 0x0000;
    // Setup MBC ROM banking pointers
    roms = (Uint8 **)SDL_malloc(sizeof(Uint8*) * romCount);
    for (int i = 0; i < romCount; i++) {
        roms[i] = &gamerom[addr];
        addr += romSizeKB * 1024;
    }
    
    // Set active ROM banks: typically bank 0 and the last bank (if available)
    romActive[0] = 0;
    romActive[1] = romCount - 1;

    // 0149 — RAM size
    int ramCounts[] = {0, -1, 1, 4, 16, 8};
    ramSizeKB = 8;
    ramCount = ramCounts[gamerom[0x0149]];
    
    // Setup MBC RAM banking pointers
    rams = (Uint8 **)SDL_malloc(sizeof(Uint8*) * ramCount);
    for (int i = 0; i < ramCount; i++) {
        rams[i] = &gamerom[addr];
        addr += ramSizeKB * 1024;
    }

    // Set active RAM bank: typically bank 0
    ramActive = 0;
}