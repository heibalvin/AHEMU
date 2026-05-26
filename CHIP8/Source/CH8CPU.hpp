/**
 * @file CH8CPU.hpp
 * @brief Declaration of the CH8CPU class, which emulates the CPU of a CHIP-8 system.
 * @details This class handles the fetch-decode-execute cycle, opcode processing, and register management.
 */

#pragma once
#include "CH8COM.hpp"
#include "CH8OPC.hpp"

/**
 * @brief The CH8CPU class emulates the Central Processing Unit of a CHIP-8 system.
 *        It inherits from CH8COM (common communication interface) and implements the core
 *        functionality of the CPU, including registers, timers, and opcode execution.
 */
class CH8CPU : public CH8COM {
public:
    /**
     * @brief Core Hardware Architecture Space Registers
     *        V[0..F] are the 16 general purpose 8-bit registers.
     *        I is the 16-bit index register.
     *        PC is the program counter.
     *        SP is the stack pointer.
     *        STACK is the stack array for subroutine calls.
     */
    Uint8  V[16];
    Uint16 I;
    Uint16 PC;
    Uint8  SP;
    Uint16 STACK[16];

    /**
     * @brief Delay timer (decrements at 60Hz when >0)
     */
    Uint8  DELAY_TIMER;
    /**
     * @brief Sound timer (decrements at 60Hz and produces sound when >0)
     */
    Uint8  SOUND_TIMER;

private:
    /**
     * @brief Holds the current opcode (2 bytes) during execution
     */
    Uint16 opc;

    /**
     * @brief Pointer to the opcode handler table (set in buildOpcodesTable)
     */
    CH8OPC* opcode = nullptr;

    /**
     * @brief Size of the opcode handler table
     */
    static const int TABLE_SIZE = 17; 

    /**
     * @brief Table of opcode handlers (function pointers or functors)
     */
    CH8OPC opcodes[TABLE_SIZE];

public:
    /**
     * @brief Constructs a CH8CPU instance.
     * @param parentEmu Pointer to the parent CHIP-8 emulator instance (for accessing memory, display, etc.)
     */
    CH8CPU(CH8EMU* parentEmu);
    virtual ~CH8CPU() = default;

    /**
     * @brief Powers on the CPU, initializing registers and memory.
     */
    void powerOn() override;
    /**
     * @brief Powers off the CPU, resetting state.
     */
    void powerOff() override;
    /**
     * @brief Resets the CPU to its initial powered-on state.
     */
    void reset() override;
    /**
     * @brief Executes a single instruction cycle (fetch, decode, execute).
     */
    void step() override;

    /**
     * @brief Fetches the next opcode from memory (at address PC) and increments PC.
     */
    void fetch();
    /**
     * @brief Decodes the fetched opcode and sets up the opcode handler.
     * @return True if the opcode is valid and a handler was found, false otherwise.
     */
    bool decode();
    /**
     * @brief Executes the opcode handler for the decoded opcode.
     */
    void execute();

private:
    /**
     * @brief Builds the opcode handler table mapping opcodes to their respective handler functions.
     */
    void buildOpcodesTable();

    // Core Opcode Command Bindings Matrix
    /**
     * @brief Handler for 0x00E0 - Clears the display.
     */
    void opCLS();
    /**
     * @brief Handler for 0x00EE - Returns from a subroutine.
     */
    void opRET();
    /**
     * @brief Handler for 0x1nnn - Jumps to address nnn.
     */
    void opJP();
    /**
     * @brief Handler for 0x2nnn - Calls subroutine at nnn.
     */
    void opCALL();
    /**
     * @brief Handler for 0x3xkk - Skips next instruction if Vx == kk.
     */
    void opSEByte();
    /**
     * @brief Handler for 0x4xkk - Skips next instruction if Vx != kk.
     */
    void opSNEByte();
    /**
     * @brief Handler for 0x5xy0 - Skips next instruction if Vx == Vy.
     */
    void opSEReg();
    /**
     * @brief Handler for 0x6xkk - Sets Vx = kk.
     */
    void opLDByte();
    /**
     * @brief Handler for 0x7xkk - Adds kk to Vx.
     */
    void opADDByte();
    /**
     * @brief Handler for 0x8xy0-0x8xyE - Performs arithmetic operations between Vx and Vy.
     */
    void opArithmetic();
    /**
     * @brief Handler for 0x9xy0 - Skips next instruction if Vx != Vy.
     */
    void opSNEReg();
    /**
     * @brief Handler for 0xAnnn - Sets I = nnn.
     */
    void opLDI();
    /**
     * @brief Handler for 0xBnnn - Jumps to address nnn + V0.
     */
    void opJPV0();
    /**
     * @brief Handler for 0xCxkk - Sets Vx to random byte AND kk.
     */
    void opRND();
    /**
     * @brief Handler for 0xDxyn - Draws sprite at (Vx, Vy) with n bytes of sprite data starting at I.
     */
    void opDRW();
    /**
     * @brief Handler for 0xEx9E/0xA1 - Handles keyboard input (skip if key pressed/not pressed).
     */
    void opKeyboard();
    /**
     * @brief Handler for 0xFx07-0xFx65 - Handles timer and memory operations.
     */
    void opTimersAndMemory();
    /**
     * @brief Handler for unknown/invalid opcodes.
     */
    void opInvalid();
};
