#pragma once
#include <SDL3/SDL.h>

class CH8CPU;

// Type definition for the concrete function pointer that executes the instruction
typedef void (CH8CPU::*OpcodeHandler)(Uint16 opcode);

struct CH8OPC {
    const char* mnemonic;    // Human-readable string representation (e.g., "LD Vx, byte")
    Uint16      mask;        // Bitmask used to match the pattern (e.g., 0xF000)
    Uint16      id;          // Unique identifying signature pattern (e.g., 0x6000)
    OpcodeHandler handler;   // Direct member function pointer to execute the logic
    Uint8       length;      // Instruction byte length (traditionally 2 bytes)
    Uint8       cycles;      // Virtual clock execution duration (useful for scheduling)
};