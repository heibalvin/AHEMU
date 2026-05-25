#include <SDL3/SDL.h>
#include "SDLEMU.hpp"

/**
 * Main Application Entry Point.
 * Orchestrates platform initialization, parses execution line parameters,
 * and passes targeting control down to the game loop matrix.
 */
int main(int argc, char* argv[]) {
    // 1. Establish standard target fallbacks if no argument is supplied
    const char* targetRomName = "1-chip8-logo.ch8";

    // 2. Overwrite standard target if an explicit path parameter is provided
    if (argc >= 2) {
        targetRomName = argv[1];
    }

    SDL_Log("Booting Core Master Engine Execution Sequence...");
    SDL_Log("Targeting ROM: %s", targetRomName);

    // 3. Stack-allocate the main SDLEMU game framework loop container
    SDLEMU game;

    // 4. Initialize physical hardware windowing and audio device handles
    if (!game.init()) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Fatal Boot Error: Game layer initialization failed.");
        return -1;
    }

    // 5. Ingest ROM and handover thread scheduling to the master loop execution phase
    game.run(targetRomName);

    // 6. Explicitly release window contexts and power down hardware motherboard lanes
    game.release();

    SDL_Log("Core Master Engine safely powered down. Exiting cleanly.");
    return 0;
}