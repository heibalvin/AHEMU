#include "GBAPU.h"

GBAPU::GBAPU(GBEMU *emu) 
    : GBCOM(emu) {
	
}

GBAPU::~GBAPU() {
	
}

void GBAPU::powerOn() {
	
 }

void GBAPU::powerOff() {
	
 }

void GBAPU::reset() { 

}

Uint8 GBAPU::read(Uint16 addr) {
	return 0x00;
}
    
void GBAPU::write(Uint16 addr, Uint8 value) {
	
}

