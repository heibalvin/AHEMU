#include "CH8BUS.hpp"

CH8BUS::CH8BUS(CH8EMU* parentEmu) : CH8COM(parentEmu) {
    reset();
}

void CH8BUS::powerOn() {
    reset();
}

void CH8BUS::powerOff() {
    clearRAM();
}

void CH8BUS::reset() {
    clearRAM();
}

void CH8BUS::clearRAM() {
    SDL_memset(RAM, 0, sizeof(RAM));
}

Uint8 CH8BUS::read(Uint16 address) const {
    return (address < 4096) ? RAM[address] : 0;
}

void CH8BUS::write(Uint16 address, Uint8 value) {
    if (address < 4096) {
        RAM[address] = value;
    }
}