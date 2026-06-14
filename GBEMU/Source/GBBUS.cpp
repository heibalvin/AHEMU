#include "GBBUS.h"

GBBUS::GBBUS(GBEMU *emu) 
    : GBCOM(emu) {
	memory = (Uint8 *)SDL_malloc(0xFFFF * sizeof(Uint8));
}

GBBUS::~GBBUS() {
	if (memory != nullptr) {
		SDL_free(memory);
		memory = nullptr;
	}
}

void GBBUS::powerOn() {
	
 }

void GBBUS::powerOff() {
	
 }

void GBBUS::reset() { }


Uint8 GBBUS::read(Uint16 address) {
    return memory[address];
}

void GBBUS::write(Uint16 address, Uint8 value) {
    memory[address] = value;
}

Uint16 GBBUS::readLE(Uint16 address) {
    return (Uint16(memory[address]) | (Uint16(memory[address + 1]) << 8));
}

void GBBUS::writeLE(Uint16 address, Uint16 value) {
	memory[address] = Uint8(value & 0x00FF);
	memory[address + 1] = Uint8((value & 0xFF00) >> 8);
}

Uint16 GBBUS::readBE(Uint16 address) {
    return ((Uint16(memory[address ]) << 8) | Uint16(memory[address + 1]));
}

void GBBUS::writeBE(Uint16 address, Uint16 value) {
	memory[address] = Uint8((value & 0xFF00) >> 8);
	memory[address + 1] = Uint8(value & 0x00FF);
}