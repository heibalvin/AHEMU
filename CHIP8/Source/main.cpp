#include "SDLEMU.hpp"

/**
 * Main Application Entry Point.
 * Orchestrates platform initialization, parses execution line parameters,
 * and passes targeting control down to the game loop matrix.
 */
int main(int argc, char* argv[]) {
    const char* romTarget = "flags.ch8";
    if (argc > 1) {
        romTarget = argv[1];
    }

    {
        // 1. Physically manufacture the console unit (Initializes SDL Window/Graphics/Audio)
        SDLEMU console; 
        
        // 2. Slide the cartridge into the slot
        if (console.insertRom(romTarget)) {
            
            // 3. Flip the physical power switch ON (Resets hardware, maps/prepares cartridge)
            console.powerOn();
            
            // 4. Run the machine execution cycle
            console.run();
            
            // 5. Flip the power switch OFF (Wipes volatile memory, turns off screen)
            console.powerOff();
        }
    } // 6. Console is destroyed (De-allocates hardware, calls SDL_Quit safely)

    return 0;
}