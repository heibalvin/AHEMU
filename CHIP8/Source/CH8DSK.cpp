#include "CH8DSK.hpp"

CH8DSK::CH8DSK(CH8EMU* parentEmu) : 
    CH8COM(parentEmu), 
    romStorage(nullptr), 
    romStorageSize(0) 
{}

CH8DSK::~CH8DSK() {
    // Persistent: Memory is explicitly freed ONLY when the emulator is entirely destroyed
    if (romStorage) {
        SDL_free(romStorage);
        romStorage = nullptr;
    }
    romStorageSize = 0;
}

void CH8DSK::powerOn() {
    // Persistent: Do not touch storage structures during state/power spikes
}

void CH8DSK::powerOff() {
    // Persistent: Do not touch storage structures during state/power spikes
}

void CH8DSK::reset() {
    // Persistent: Do not touch storage structures during state/power spikes
}

bool CH8DSK::insertRom(const Uint8* datas, size_t size) {
    if (!datas || size == 0) return false;

    // Free any cartridge previously slotted into the disk deck
    if (romStorage) {
        SDL_free(romStorage);
        romStorage = nullptr;
    }

    // Allocate a raw, zero-overhead memory segment matching the exact ROM layout footprint
    romStorage = static_cast<Uint8*>(SDL_malloc(size));
    if (!romStorage) {
        romStorageSize = 0;
        return false;
    }

    // Clone the source bits directly into your persistent storage bank
    SDL_memcpy(romStorage, datas, size);
    romStorageSize = size;

    return true;
}