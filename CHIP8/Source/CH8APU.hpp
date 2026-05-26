/**
 * @file CH8APU.hpp
 * @brief Declaration of the CH8APU class, which emulates the audio processing unit of a CHIP-8 system.
 * @details The APU is responsible for generating a beep sound when the sound timer is active.
 */

#pragma once
#include "CH8COM.hpp"

/**
 * @brief The CH8APU class emulates the Audio Processing Unit of a CHIP-8 system.
 *        It handles the sound timer and produces a beep when the sound timer is non-zero.
 */
class CH8APU : public CH8COM {
private:
    /**
     * @brief Flag indicating whether the buzzer is currently active (sound timer > 0).
     */
    bool buzzerActive = false;

public:
    /**
     * @brief Constructs a CH8APU instance.
     * @param parentEmu Pointer to the parent CHIP-8 emulator instance.
     */
    CH8APU(CH8EMU* parentEmu);
    /** @brief Destructor (default). */
    ~CH8APU() = default;

    /**
     * @brief Powers on the APU, initializing the buzzer state.
     */
    void powerOn() override;
    /**
     * @brief Powers off the APU, turning off the buzzer.
     */
    void powerOff() override;
    /**
     * @brief Resets the APU to its initial powered-on state.
     */
    void reset() override;
    /**
     * @brief Steps the APU (called per CPU cycle) to update the buzzer state based on the sound timer.
     *        This function is typically called from the CPU's step function or the emulator's update loop.
     */
    void step() override;
    /**
     * @brief Stops the buzzer immediately (used when sound timer reaches zero).
     */
    void stop();
    /**
     * @brief Checks if the buzzer is currently active.
     * @return True if the buzzer is active, false otherwise.
     */
    bool isBuzzerActive() const;
};
