/**
 * @file CH8COM.hpp
 * @brief Declaration of the CH8COM abstract base class, which defines the common interface for all CHIP-8 hardware components.
 * @details This class provides a standardized interface that all hardware components (CPU, memory, PPU, etc.) must implement.
 *          It includes methods for power management, reset, and communication via the system bus.
 */

#pragma once
#include <SDL3/SDL.h>

class CH8EMU;

/**
 * @brief The CH8COM class is an abstract base class that defines the common interface for all CHIP-8 hardware components.
 *        All hardware components in the emulator (CPU, PPU, APU, etc.) inherit from this class.
 *        It provides standardized methods for power management, reset, and system bus communication.
 */
class CH8COM {
protected:
    /**
     * @brief Pointer to the parent CHIP-8 emulator instance.
     *        This allows components to communicate with each other through the emulator's bus system.
     */
    CH8EMU* emu = nullptr;

public:
    /** @brief Default constructor. */
    CH8COM() = default;
    /**
     * @brief Constructs a CH8COM component with a reference to the parent emulator.
     * @param parentEmu Pointer to the parent CHIP-8 emulator instance.
     */
    CH8COM(CH8EMU* parentEmu) : emu(parentEmu) {}
    /** @brief Virtual destructor for proper cleanup of derived classes. */
    virtual ~CH8COM() = default;

    /**
     * @brief Powers on the component.
     *        This pure virtual function must be implemented by all derived classes.
     *        Called when the emulator is powered on.
     */
    virtual void powerOn() = 0;
    /**
     * @brief Powers off the component.
     *        This pure virtual function must be implemented by all derived classes.
     *        Called when the emulator is powered off.
     */
    virtual void powerOff() = 0;
    /**
     * @brief Resets the component to its initial state.
     *        This pure virtual function must be implemented by all derived classes.
     *        Called when the emulator is reset.
     */
    virtual void reset() = 0;
    /**
     * @brief Steps the component (optional).
     *        This virtual function can be overridden by derived classes for per-cycle updates.
     *        Default implementation does nothing.
     */
    virtual void step() {}

    /**
     * @brief Reads a byte from the system bus at the specified address.
     * @param address The 16-bit memory address to read from.
     * @return The byte value at the specified address.
     * @details This function delegates to the emulator's bus system.
     */
    Uint8 readBus(Uint16 address) const;
    /**
     * @brief Writes a byte to the system bus at the specified address.
     * @param address The 16-bit memory address to write to.
     * @param value The byte value to write.
     * @details This function delegates to the emulator's bus system.
     */
    void writeBus(Uint16 address, Uint8 value);
};
