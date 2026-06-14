#ifndef GBEMU_H
#define GBEMU_H

#include "GBBUS.h"
#include "GBDSK.h"

class GBEMU {
    friend class GBAPP; // Allows GBAPP to access isRunning
private:
    GBBUS bus;
    GBDSK dsk;
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