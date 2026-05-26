/**
 * @file CH8OPC.hpp
 * @brief Declaration of the CH8OPC struct, which defines an opcode handler entry in the CPU's opcode table.
 * @details This struct maps opcode bit patterns to their corresponding handler functions in the CH8CPU class.
 */

#pragma once
#include <SDL3/SDL.h>

class CH8CPU;

/**
 * @brief The CH8OPC struct defines an entry in the CPU's opcode handler table.
 *        Each entry specifies:
 *        - The mnemonic name of the opcode
 *        - A bitmask used for matching opcodes
 *        - The opcode identifier value
 *        - A pointer to the member function that handles this opcode
 *        - The length of the opcode in bytes
 *        - The number of CPU cycles required to execute this opcode
 */
struct CH8OPC {
    /**
     * @brief Human-readable mnemonic for the opcode (e.g., "CLS", "RET", "JP").
     */
    const char* mnemonic;
    /**
     * @brief Bitmask used to match opcodes against this entry.
     *        The opcode is ANDed with this mask, and the result must equal 'id' for a match.
     */
    Uint16 mask;
    /**
     * @brief The opcode identifier value that must match after applying the mask.
     */
    Uint16 id;
    /**
     * @brief Pointer to the member function in CH8CPU that handles this opcode.
     */
    void (CH8CPU::*handler)();
    /**
     * @brief Length of the opcode in bytes (typically 2 for CHIP-8).
     */
    Uint8 length;
    /**
     * @brief Number of CPU cycles required to execute this opcode.
     */
    Uint8 cycles;
};
