#pragma once
#include "CH8COM.hpp"
#include "CH8OPC.hpp"

class CH8CPU : public CH8COM {
public:
    // Core Hardware Architecture Space Registers
    Uint8  V[16];
    Uint16 I;
    Uint16 PC;
    Uint8  SP;
    Uint16 STACK[16];

    Uint8  DELAY_TIMER;
    Uint8  SOUND_TIMER;

private:
    Uint16 opc; // Holds both instruction bytes
    
    // Lightened execution reference pointer
    CH8OPC* opcode = nullptr;

    static const int TABLE_SIZE = 17; 
    CH8OPC opcodes[TABLE_SIZE];

public:
    CH8CPU(CH8EMU* parentEmu);
    virtual ~CH8CPU() = default;

    void powerOn() override;
    void powerOff() override;
    void reset() override;
    void step() override;

    // Execution Pipeline Processing Phases
    void fetch();
    bool decode();
    void execute();

private:
    void buildOpcodesTable();

    // Core Opcode Command Bindings Matrix
    void opCLS();
    void opRET();
    void opJP();
    void opCALL();
    void opSEByte();
    void opSNEByte();
    void opSEReg();
    void opLDByte();
    void opADDByte();
    void opArithmetic();
    void opSNEReg();
    void opLDI();
    void opJPV0();
    void opRND();
    void opDRW();
    void opKeyboard();
    void opTimersAndMemory();
    void opInvalid();
};