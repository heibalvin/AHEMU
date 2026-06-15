#ifndef GBCOM_H
#define GBCOM_H

#include <SDL3/SDL.h>

// Forward declaration of the parent motherboard class
class GBEMU;

class GBCOM {
protected:
    GBEMU *emu;

public:
    explicit GBCOM(GBEMU *emu) : emu(emu) {}
    virtual ~GBCOM() {};

    virtual void powerOn() = 0;
    virtual void powerOff() = 0;
    virtual void reset() = 0;

    virtual Uint8 read(Uint16 addr) = 0;
    virtual void  write(Uint16 addr, Uint8 value) = 0;
};

#endif // GBCOM_H