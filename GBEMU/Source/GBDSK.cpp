#include "GBDSK.h"
#include "GBEMU.h"

GBDSK::GBDSK(GBEMU *emu)
    : GBCOM(emu), romData(nullptr), romSize(0) {}

GBDSK::~GBDSK() {
    if (romData) {
		SDL_free(romData);
		romData = nullptr;
		romSize = 0;
	}
}

void GBDSK::powerOn() { }

void GBDSK::powerOff() { }

void GBDSK::reset() { }

void GBDSK::load(const Uint8* data, size_t size) {
	romSize = size;
    romData = (Uint8 *)SDL_malloc(romSize * sizeof(Uint8));
    SDL_memcpy(romData, data, romSize);

	SDL_Log("DSK: loaded ROM with title \()");
}
