/**
 * @file CH8BUS.hpp
 * @brief Declaration of the CH8BUS class, which emulates the system bus and memory of a CHIP-8 system.
 * @details The bus provides a 4KB memory space (RAM) for storing the ROM, program data, and hardware registers.
 */

#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

/**
 * @brief The CH8BUS class emulates the system bus of a CHIP-8 system.
 *        It provides a 4KB RAM array and methods to read from and write to memory.
 */
class CH8BUS : public CH8COM {
public:
    /**
     * @brief The 4KB memory space (4096 bytes) of the CHIP-8 system.
     *        This memory stores the ROM (loaded at 0x200), program data, stack, and hardware registers.
     */
    Uint8 RAM[4096]; 

    /**
     * @brief Constructs a CH8BUS instance.
     * @param parentEmu Pointer to the parent CHIP-8 emulator instance.
     */
    CH8BUS(CH8EMU* parentEmu);
    /** @brief Destructor (default). */
    ~CH8BUS() = default;

    /**
     * @brief Powers on the bus, initializing the memory (typically by clearing RAM).
     */
    void powerOn() override;
    /**
     * @brief Powers off the bus.
     */
    void powerOff() override;
    /**
     * @brief Resets the bus to its initial powered-on state (clears RAM).
     */
    void reset() override;

    /**
     * @brief Clears the entire RAM by setting all bytes to 0.
     */
    void clearRAM();
    /**
     * @brief Reads a byte from the specified memory address.
     * @param address The 16-bit memory address to read from (0x0000 to 0x0FFF).
     * @return The byte value at the specified address.
     */
    Uint8 read(Uint16 address) const;
    /**
     * @brief Writes a byte to the specified memory address.
     * @param address The 16-bit memory address to write to (0x0000 to 0x0FFF).
     * @param value The byte value to write.
     */
    void write(Uint16 address, Uint8 value);
};
