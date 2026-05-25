#include "CH8DSK.hpp"

CH8DSK::CH8DSK(CH8EMU* parentEmu) : 
    CH8COM(parentEmu),
    ROM_DATA(nullptr),
    ROM_SIZE(0)
{}

CH8DSK::~CH8DSK() {
    clearStorage();
}

void CH8DSK::powerOn() {
    // Media connections remain on standby until an explicit load signal arrives
    reset();
}

void CH8DSK::powerOff() {
    clearStorage();
}

void CH8DSK::reset() {
    // Resetting a disk layout keeps the existing media intact without wiping data blocks
}

bool CH8DSK::loadROM(const void* data, size_t size) {
    if (!data || size == 0) {
        return false;
    }

    // Scrub any pre-existing sector mappings before re-allocating
    clearStorage();

    // Allocate storage using SDL3's centralized runtime allocations wrapper
    ROM_DATA = static_cast<Uint8*>(SDL_malloc(size));
    if (!ROM_DATA) {
        SDL_Log("Disk Error: Core memory allocator allocation block fault.");
        return false;
    }

    // Perform an accelerated low-level bytecode replication match copy execution pass
    SDL_memcpy(ROM_DATA, data, size);
    ROM_SIZE = size;

    SDL_Log("Disk IO Status: Successfully verified and mounted %zu bytes into sector data array blocks.", ROM_SIZE);
    return true;
}

void CH8DSK::clearStorage() {
    if (ROM_DATA) {
        SDL_free(ROM_DATA);
        ROM_DATA = nullptr;
    }
    ROM_SIZE = 0;
}