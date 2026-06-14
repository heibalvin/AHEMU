#ifndef GBDSK_H
#define GBDSK_H

#include <SDL3/SDL.h>
#include "GBCOM.h"

class GBDSK: GBCOM {
private:
	Uint8 *romData;
	size_t romSize;

public:
    GBDSK(GBEMU *emu);
    ~GBDSK();

	void powerOn();
    void powerOff();
    void reset();

	void load(const Uint8* data, size_t size);
};

#endif // GBDSK_H