/**
 * @file CH8PPU.hpp
 * @brief Declaration of the CH8PPU class, which emulates the pixel processing unit (display) of a CHIP-8 system.
 * @details This class handles rendering graphics to the CHIP-8's 64x32 pixel display.
 *          It manages the framebuffer and provides functions for drawing sprites and clearing the screen.
 */

#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

/**
 * @brief The CH8PPU class emulates the Pixel Processing Unit of a CHIP-8 system.
 *        It handles all aspects of the display, including the framebuffer,
 *        sprite drawing, and screen clearing operations.
 *        The display resolution is 64 pixels wide by 32 pixels high.
 */
class CH8PPU : public CH8COM {
public:
    /** @brief Width of the CHIP-8 display in pixels (64). */
    const int CH8_WIDTH  = 64;
    /** @brief Height of the CHIP-8 display in pixels (32). */
    const int CH8_HEIGHT = 32;

    /**
     * @brief The framebuffer storing pixel data for the display.
     *        Each pixel is represented by a 32-bit value (typically ARGB8888).
     *        The buffer is organized as a 1D array of width*height elements.
     */
    Uint32 frameBuffer[64 * 32];

    /**
     * @brief Constructs a CH8PPU instance.
     * @param parentEmu Pointer to the parent CHIP-8 emulator instance.
     */
    CH8PPU(CH8EMU* parentEmu);
    /** @brief Destructor (default). */
    ~CH8PPU() = default;

    /**
     * @brief Powers on the PPU, initializing the framebuffer (typically cleared).
     */
    void powerOn() override;
    /**
     * @brief Powers off the PPU.
     */
    void powerOff() override;
    /**
     * @brief Resets the PPU to its initial powered-on state (clears the framebuffer).
     */
    void reset() override;

    /**
     * @brief Clears the entire framebuffer, setting all pixels to off (color 0).
     */
    void clearScreen();
    /**
     * @brief Draws a sprite at the specified coordinates.
     * @param xCoordinate X position to draw the sprite (0-63).
     * @param yCoordinate Y position to draw the sprite (0-31).
     * @param spriteDataPointer Pointer to the sprite data bytes.
     * @param spriteHeight Height of the sprite in bytes (each byte represents 8 pixels).
     * @return The number of pixels that were turned off due to collision (0 or 1).
     *         This is used for VF register collision detection.
     */
    Uint8 writeSprite(Uint8 xCoordinate, Uint8 yCoordinate, const Uint8* spriteDataPointer, Uint8 spriteHeight);
};
