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
    Uint16 currentOpcode;
    CH8OPC matchedInstruction;

    static const int TABLE_SIZE = 17; 
    CH8OPC opcodesTable[TABLE_SIZE];

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

    // Debugging Inspection Accessors
    const CH8OPC* getOpcodeTableInstance() const;
    int getOpcodeTableSize() const;
    CH8OPC getCurrentDebugFrame() const;

private:
    void buildOpcodesTable();

    // Core Opcode Command Bindings Matrix
    void opCLS(Uint16 op);
    void opRET(Uint16 op);
    void opJP(Uint16 op);
    void opCALL(Uint16 op);
    void opSEByte(Uint16 op);
    void opSNEByte(Uint16 op);
    void opSEReg(Uint16 op);
    void opLDByte(Uint16 op);
    void opADDByte(Uint16 op);
    void opArithmetic(Uint16 op);
    void opSNEReg(Uint16 op);
    void opLDI(Uint16 op);
    void opJPV0(Uint16 op);
    void opRND(Uint16 op);
    void opDRW(Uint16 op);
    void opKeyboard(Uint16 op);
    void opTimersAndMemory(Uint16 op);
    void opInvalid(Uint16 op);
};