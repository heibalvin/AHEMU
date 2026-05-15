#ifndef GBCPU_HPP
#define GBCPU_HPP

#include <SDL3/SDL.h>
#include "gbcomponent.hpp"

class GBEMU;

class GBCPU : public GBComponent {
public:
    explicit GBCPU(GBEMU* emu);
    ~GBCPU();

    void powerOn() override;
    void step() override;

    void fetch();
    void decode();
    void execute();

private:
    friend class GBEMU;
    friend class GBBUS;

    // High RAM (HRAM) and Work RAM (WRAM) management
    Uint8 *hram = NULL; // High RAM (HRAM) for the last 128 bytes of memory
    Uint8 *wrams[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    int wramCount = 1;
    int wramActive[2] = { 0, 0 };

    // Registers
    Uint16 AF = 0x0000;
    Uint16 BC = 0x0000;
    Uint16 DE = 0x0000;
    Uint16 HL = 0x0000;
    Uint16 SP = 0x0000; // Stack Pointer
    Uint16 PC = 0x0000; // Program Counter

    // Flags
    const Uint8 FLAG_Z = 0x80; // Zero Flag
    const Uint8 FLAG_N = 0x40; // Subtract Flag
    const Uint8 FLAG_H = 0x20; // Half Carry Flag
    const Uint8 FLAG_C = 0x10; // Carry Flag
};

#endif /* GBCPU_HPP */