#ifndef GBCPU_H
#define GBCPU_H

#include <SDL3/SDL.h>
#include "GBCOM.h"

// Forward declaration to avoid circular dependencies
class GBEMU; 
class GBCPU;

typedef void (GBCPU::*GBOPC_FUNC)();

struct GBOPC {
    const char* mnemonic;
    Uint8 length;
    Uint8 cycles;
    GBOPC_FUNC execute;
};

class GBCPU : public GBCOM {
private:
    Uint8 HRAM[0x80];   // 128 bytes
    struct CPU_REG {
        // Leave the struct anonymous; you can still access AF.A and AF.F directly
        union { struct { Uint8 F, A; }; Uint16 AF; };
        union { struct { Uint8 C, B; }; Uint16 BC; };
        union { struct { Uint8 E, D; }; Uint16 DE; };
        union { struct { Uint8 L, H; }; Uint16 HL; };
        Uint16 SP;
        Uint16 PC;
    } regs;

    const GBOPC opcodes[256] = {
        {"NOP", 1, 4, &GBCPU::NOP},                 // 0x00
        {"LD BC, d16", 3, 12, &GBCPU::LD_BC_d16},   // 0x01
        { "PUSH BC", 1, 16, &GBCPU::PUSH_BC }, // 0xC5
        { "POP BC",  1, 12, &GBCPU::POP_BC  }, // 0xC1
        { "CALL nn", 3, 24, &GBCPU::CALL_nn }, // 0xCD
        { "RET",     1, 16, &GBCPU::RET     }, // 0xC9
        { "EI",      1, 4,  &GBCPU::EI      }, // 0xFB (Enable Interrupts)
        { "DI",      1, 4,  &GBCPU::DI      }, // 0xF3 (Disable Interrupts)
        // ... define all 256 entries
        {nullptr, 0, 0, nullptr}              // Placeholder for unknown/unimplemented
    };
    
    const GBOPC* opcode = nullptr;      // Global-ish state for the current step
    Uint8 bytes[3] = {0, 0, 0};         // Holds opcode & operands
    Uint16 nextPC;                      // Staged PC update

public:
    explicit GBCPU(GBEMU &emu);

    // GBCOM Interface
    void powerOn() override;
    void powerOff() override;
    void reset() override;
    Uint8 read(Uint16 addr) override;
    void  write(Uint16 addr, Uint8 value) override;

    // CPU Phases
    int step();
    void handleInterrupts();
    void fetch();
    void decode();
    void execute();

    // Opcode Functions
    void NOP();
    void LD_BC_d16();
    void PUSH_BC();
    void POP_BC();
    void CALL_nn();
    void RET();
    void EI();
    void DI();
};

#endif