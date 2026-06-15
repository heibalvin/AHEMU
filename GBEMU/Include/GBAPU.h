#ifndef GBAPU_H
#define GBAPU_H

#include <SDL3/SDL.h>
#include "GBCOM.h"

class GBAPU: GBCOM {
private:

public:
    GBAPU(GBEMU *emu);
    ~GBAPU();

	void powerOn();
    void powerOff();
    void reset();
    Uint8 read(Uint16 addr);
    void  write(Uint16 addr, Uint8 value);
};

#endif // GBAPU_H