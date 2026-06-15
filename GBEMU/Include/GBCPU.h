#ifndef GBCPU_H
#define GBCPU_H

#include <SDL3/SDL.h>
#include "GBCOM.h"

class GBCPU: GBCOM {
private:
    Uint8 WRAM[4 * 1024];

public:
    GBCPU(GBEMU *emu);
    ~GBCPU();

	void powerOn();
    void powerOff();
    void reset();
    Uint8 read(Uint16 addr);
    void  write(Uint16 addr, Uint8 value);

};

#endif // GBCPU_H