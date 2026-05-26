/**
 * @file CH8EMU.hpp
 * @brief Declaration of the CH8EMU class, which represents the entire CHIP-8 emulator system.
 * @details This class composes all the subsystems (BUS, CPU, PPU, KEY, APU, DSK) and coordinates
 *          their operation through a main loop that handles timing and synchronization.
 */

#pragma once
#include <SDL3/SDL.h>
#include "CH8BUS.hpp"
#include "CH8CPU.hpp"
#include "CH8PPU.hpp"
#include "CH8KEY.hpp"
#include "CH8APU.hpp"
#include "CH8DSK.hpp"
#include "CH8COM.hpp"


/**
 * @brief The CH8EMU class represents the complete CHIP-8 emulator.
 *        It inherits from CH8COM (common communication interface) and contains instances of
 *        all the major components: bus, CPU, PPU, keyboard, APU, and disk (ROM loader).
 *        It also manages timing for frames per second (FPS) and updates per second (UPS).
 */
class CH8EMU : public CH8COM {
public:
    /** @brief The system bus for communication between components. */
    CH8BUS bus;
    /** @brief The central processing unit. */
    CH8CPU cpu;
    /** @brief The pixel processing unit (handles display and graphics). */
    CH8PPU ppu;
    /** @brief The keyboard input handler. */
    CH8KEY key;
    /** @brief The audio processing unit (handles beep/tone). */
    CH8APU apu;
    /** @brief The disk system (handles ROM loading). */
    CH8DSK dsk;

private:
    /** @brief Target frames per second (for video refresh). */
    double fpsTarget;
    /** @brief Target updates per second (for CPU timers and instruction rate). */
    double upsTarget;

    /** @brief Accumulator for frame timing (used to regulate video output). */
    double frameAccumulator;
    /** @brief Accumulator for update timing (used to regulate CPU steps). */
    double updateAccumulator;
    /** @brief Accumulator for hardware timer updates (delay and sound timers). */
    double hardwareTimerAccumulator;
    /** @brief Timer used for profiling (optional). */
    double profileTimer;

    /** @brief Counter for frames rendered (used to calculate actual FPS). */
    int fpsCount;
    /** @brief Counter for CPU updates (used to calculate actual UPS). */
    int upsCount;
    /** @brief Most recently calculated frames per second. */
    int fpsCalculated;
    /** @brief Most recently calculated updates per second. */
    int upsCalculated;

    /** @brief Flag indicating that a new frame is ready to be presented. */
    bool frameReadyFlag;

public:
    /**
     * @brief Constructs the emulator and initializes all components.
     */
    CH8EMU();
    /** @brief Destructor (default). */
    ~CH8EMU() = default;

    /**
     * @brief Powers on the entire emulator system.
     *        This calls powerOn() on all subcomponents.
     */
    void powerOn();
    /**
     * @brief Powers off the emulator system.
     *        This calls powerOff() on all subcomponents.
     */
    void powerOff();
    /**
     * @brief Resets the emulator to its initial powered-on state.
     *        This calls reset() on all subcomponents.
     */
    void reset();
    /**
     * @brief Executes a single emulator step (typically one CPU instruction).
     *        This calls step() on the CPU and updates timers.
     */
    void step();
    /**
     * @brief Updates the emulator state based on elapsed time.
     *        This handles frame timing, update timing, and hardware timers.
     *        @param deltaTime Time elapsed since the last update (in seconds).
     */
    void update(double deltaTime);

    /**
     * @brief Dynamically passes through to the disk system to insert a ROM.
     *        @param datas Pointer to the ROM data.
     *        @param size Size of the ROM data in bytes.
     *        @return True if the ROM was successfully inserted, false otherwise.
     */
    bool insertRom(const Uint8* datas, size_t size) { return dsk.insertRom(datas, size); }

    /**
     * @brief Sets the target frames per second.
     * @param target The desired FPS (e.g., 60.0).
     */
    void setFpsTarget(double target) { fpsTarget = target; }
    /**
     * @brief Sets the target updates per second.
     * @param target The desired UPS (e.g., 600 for CHIP-8 timers).
     */
    void setUpsTarget(double target) { upsTarget = target; }
    /**
     * @brief Gets the current target frames per second.
     * @return The target FPS.
     */
    double getFpsTarget() const { return fpsTarget; }
    /**
     * @brief Gets the current target updates per second.
     * @return The target UPS.
     */
    double getUpsTarget() const { return upsTarget; }
    /**
     * @brief Gets the most recently calculated frames per second.
     * @return The calculated FPS (updated once per second).
     */
    int getFpsCalculated() const { return fpsCalculated; }
    /**
     * @brief Gets the most recently calculated updates per second.
     * @return The calculated UPS (updated once per second).
     */
    int getUpsCalculated() const { return upsCalculated; }

    /**
     * @brief Checks if a new frame is ready to be displayed.
     * @return True if the frameReadyFlag is set, false otherwise.
     */
    bool isFrameReady() const { return frameReadyFlag; }
    /**
     * @brief Clears the frame ready flag (called after presenting the frame).
     */
    void clearFrameReadyFlag() { frameReadyFlag = false; }
};
