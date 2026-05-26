/**
 * @file CH8KEY.hpp
 * @brief Declaration of the CH8KEY class, which emulates the keyboard controller of a CHIP-8 system.
 * @details This class handles input from the host keyboard and maps it to the CHIP-8 hex keypad (0x0-0xF).
 *          It also implements the FX0A opcode which waits for a key press.
 */

#ifndef CH8KEY_HPP
#define CH8KEY_HPP

#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8EMU; // Forward declaration of parent container

/**
 * @brief The CH8KEY class emulates the keyboard controller of a CHIP-8 system.
 *        It maps the host keyboard to the CHIP-8 hex keypad and handles key press/release events.
 *        It also implements the FX0A blocking key wait opcode.
 */
class CH8KEY : public CH8COM {
public:
    /**
     * @brief Constructs a CH8KEY instance.
     * @param parentEmu Pointer to the parent CHIP-8 emulator instance.
     */
    CH8KEY(CH8EMU* parentEmu);
    /** @brief Destructor (default). */
    virtual ~CH8KEY() = default;

    // Implementation of CH8COM abstract hardware interface overrides
    /**
     * @brief Powers on the keyboard controller (initializes all keys to released state).
     */
    virtual void powerOn() override;
    /**
     * @brief Powers off the keyboard controller.
     */
    virtual void powerOff() override;
    /**
     * @brief Resets the keyboard controller to its initial powered-on state (all keys released).
     */
    virtual void reset() override;

    // Standard keypad hardware operations
    /**
     * @brief Sets the state of a specific key.
     * @param keyIndex The index of the key to set (0x0-0xF).
     * @param state The state to set (1 for pressed, 0 for released).
     */
    void setKeyState(Uint8 keyIndex, Uint8 state);
    /**
     * @brief Checks if a specific key is currently pressed.
     * @param keyIndex The index of the key to check (0x0-0xF).
     * @return True if the key is pressed, false otherwise.
     */
    bool isKeyPressed(Uint8 keyIndex) const;

    // FX0A Key release tracking subsystem latch
    /**
     * @brief Locks the key wait state for the FX0A opcode (waits for key press).
     * @param keyIndex The index of the key that was pressed to trigger the wait.
     */
    void lockKeyWait(Uint8 keyIndex);
    /**
     * @brief Clears the key wait state (used when a key is pressed during FX0A wait).
     */
    void clearKeyWait();
    /**
     * @brief Checks if the keyboard is currently waiting for a key press (FX0A opcode).
     * @return True if waiting for key press, false otherwise.
     */
    bool isCurrentlyWaiting() const;
    /**
     * @brief Gets the index of the key that is currently latched during FX0A wait.
     * @return Index of the latched key, or -1 if not waiting.
     */
    int getLatchedKey() const;

private:
    /**
     * @brief Array storing the state of each key (16 keys, indexed 0x0-0xF).
     *        Value 1 = pressed, 0 = released.
     */
    Uint8 keysState[16];      // 1 = Pressed, 0 = Released
    /**
     * @brief Flag indicating if the keyboard is currently waiting for a key press (during FX0A opcode).
     */
    bool isWaitingForKey;     // Active flag during FX0A hardware hold
    /**
     * @brief Index of the key that is currently latched during FX0A hardware hold.
     */
    int latchedKeyIndex;      // Index of locked active scanner key loop
};

#endif // CH8KEY_HPP
