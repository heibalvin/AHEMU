#include "GBCPU.h"

GBCPU::GBCPU(GBEMU *emu) 
    : GBCOM(emu) {
	
}

GBCPU::~GBCPU() {
	
}

void GBCPU::powerOn() {
	
 }

void GBCPU::powerOff() {
	
 }

void GBCPU::reset() { 

}

Uint8 GBCPU::read(Uint16 addr) {
	return WRAM[addr & 0x1000];
}
    
void GBCPU::write(Uint16 addr, Uint8 value) {
	WRAM[addr & 0x1000] = value;
}

