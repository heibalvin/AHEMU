#ifndef GBEMU_H
#define GBEMU_H

#include "GBBUS.h"
#include "GBDSK.h"
#include "GBCPU.h"
#include "GBVDP.h"
#include "GBJOY.h"
#include "GBAPU.h"

class GBEMU {
    friend class GBAPP; // Allows GBAPP to access isRunning
    friend class GBBUS;
    friend class GBVDP;
    friend class GBCPU;
private:
    GBBUS bus;
    GBDSK dsk;
    GBCPU cpu;
    GBVDP vdp;
    GBJOY joy;
    GBAPU apu;

	bool isRunning;
	int width = 160;
	int height = 144;

    // Interrupt Registers
    Uint8 IE = 0; // Interrupt Enable (0xFFFF)
    Uint8 IF = 0; // Interrupt Flag (0xFF0F)
    
    // CPU State
    bool IME = false;
    bool isIRQRequested = false;
public:
    GBEMU();
    ~GBEMU();

    bool powerOn();
    void powerOff();
    void reset();

    void step();
    void run(Uint64 elapsedTicks);
    void load(const Uint8* data, size_t size);

    const Uint32* getVDPFrameBuffer() const;
    void setJoypadButton(int button, bool pressed);

    void updateIRQRequest();
};

#endif