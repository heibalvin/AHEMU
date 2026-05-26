/**
 * @file SDLEMU.hpp
 * @brief Declaration of the SDLEMU class, which provides the SDL3 interface for the CHIP-8 emulator.
 * @details This class manages SDL window creation, rendering, input handling, and audio output.
 *          It contains the main emulator loop and handles all platform-specific SDL interactions.
 */

#pragma once
#include <SDL3/SDL.h>
#include "CH8EMU.hpp"

/**
 * @brief The SDLEMU class provides the SDL3 interface layer for the CHIP-8 emulator.
 *        It manages SDL window creation, rendering, input handling, and audio output.
 *        This class contains the main emulator loop and integrates all components:
 *        - SDL window, renderer, and texture management
 *        - Event processing and input mapping
 *        - Audio streaming for beep sound
 *        - Filesystem access for ROM loading and splash screen
 *        - Main emulator loop with timing synchronization
 */
class SDLEMU {
private:
    /** @brief The CHIP-8 emulator instance that this SDL interface controls. */
    CH8EMU emu;

    // SDL Interface Context Elements
    /** @brief SDL window handle for the emulator display. */
    SDL_Window* window;
    /** @brief SDL renderer handle for drawing to the window. */
    SDL_Renderer* renderer;
    /** @brief SDL texture handle for displaying the CHIP-8 framebuffer. */
    SDL_Texture* texture;
    /** @brief Flag indicating whether the emulator main loop is running. */
    bool          isRunning;

    // Platform-Independent Asset Path Tracks
    /** @brief Pointer to the project path string (dynamically allocated). */
    char* projectPath;
    /** @brief Pointer to the resource path string (dynamically allocated). */
    char* resourcePath;

    /** @brief SDL audio stream for generating beep sound. */
    SDL_AudioStream* audioStream;  
    /** @brief Flag indicating whether the audio device is currently playing. */
    bool audioDevicePlaying;

public:
    /**
     * @brief Constructor - initializes SDL members but does not initialize SDL itself.
     *        SDL initialization is done in the SDLinit() method.
     */
    SDLEMU();   
    /**
     * @brief Destructor - cleans up SDL resources.
     */
    ~SDLEMU();  

    // Delete copy constructor and assignment operator to prevent copying of SDL resources
    SDLEMU(const SDLEMU&) = delete;
    SDLEMU& operator=(const SDLEMU&) = delete;

    // Physical Media & Power Subsystem Interface Matrix
    /**
     * @brief Loads a ROM file into the emulator.
     * @param romName Name of the ROM file to load (relative to resources directory).
     * @return True if the ROM was successfully loaded, false otherwise.
     */
    bool insertRom(const char* romName); 
    /**
     * @brief Powers on the emulator system.
     *        This initializes SDL, loads the splash screen, and prepares the emulator for running.
     */
    void powerOn();                      
    /**
     * @brief Starts the main emulator loop.
     *        This function handles events, updates the emulator state, and renders frames
     *        until the emulator is stopped.
     */
    void run();                          
    /**
     * @brief Powers off the emulator system.
     *        This stops the main loop and cleans up SDL resources.
     */
    void powerOff();                     

private:
    /** @brief Width of the SDL window in pixels (1280). */
    const int WINDOW_WIDTH  = 1280;
    /** @brief Height of the SDL window in pixels (640). */
    const int WINDOW_HEIGHT = 640;

    /**
     * @brief Initializes SDL and creates window, renderer, and texture.
     * @return True if SDL was successfully initialized, false otherwise.
     * @details Called by powerOn() to set up the SDL environment.
     */
    bool SDLinit();     
    /**
     * @brief Releases SDL resources (window, renderer, texture).
     * @details Called by powerOff() to clean up the SDL environment.
     */
    void SDLrelease();  

    /**
     * @brief Initializes filesystem paths for locating resources.
     *        Determines the project and resource paths based on the executable location.
     */
    void filesystem();  
    /**
     * @brief Processes SDL input events and updates the keyboard state.
     *        Maps SDL keycodes to CHIP-8 hex key values.
     */
    void input();       
    /**
     * @brief Updates the emulator state based on elapsed time.
     * @param deltaTime Time elapsed since the last update (in seconds).
     *        This delegates to CH8EMU::update() for timing synchronization.
     */
    void update(double deltaTime);
    /**
     * @brief Updates and plays audio based on the CHIP-8 sound timer state.
     *        Generates a beep sound when the sound timer is active.
     */
    void audio();
    /**
     * @brief Renders the current frame from the CHIP-8 framebuffer to the SDL window.
     *        This copies the PPU's framebuffer to the SDL texture and presents it.
     */
    void render();
};
