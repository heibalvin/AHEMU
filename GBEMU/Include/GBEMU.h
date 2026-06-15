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
    friend class GBVDP; // Add this line to allow GBVDP to access 'bus'
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
	
public:
    GBEMU();
    ~GBEMU();

    bool powerOn();
    void powerOff();
    void reset();
    int step();
    void run();
    void load(const Uint8* data, size_t size);
};

#endif