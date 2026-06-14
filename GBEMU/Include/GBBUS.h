#ifndef GBBUS_H
#define GBBUS_H

#include <SDL3/SDL.h>
#include "GBCOM.h"

class GBBUS: GBCOM {
private:
	Uint8 *memory;
public:
    GBBUS(GBEMU *emu);
    ~GBBUS();

	void powerOn();
    void powerOff();
    void reset();

    // Standard Accessors
    Uint8 read(Uint16 address);
    void  write(Uint16 address, Uint8 value);

    Uint16 readLE(Uint16 address);
    void   writeLE(Uint16 address, Uint16 value);

	Uint16 readBE(Uint16 address);
    void   writeBE(Uint16 address, Uint16 value);
};

#endif // GBBUS_H