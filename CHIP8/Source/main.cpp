#include <SDL3/SDL.h>
#include "SDLEMU.hpp"

/**
 * Main Application Entry Point.
 * Orchestrates platform initialization, parses execution line parameters,
 * and passes targeting control down to the game loop matrix.
 */
int main(int argc, char* argv[]) {
    // Gracefully handle arguments safely without static stack buffer allocations
    const char* romTarget = "flags.ch8";
    if (argc > 1) {
        romTarget = argv[1];
    }

    // Allocate on the heap or as an object instance safely scoped
    SDLEMU* app = new SDLEMU();
    
    if (app->init()) {
        app->run(romTarget);
    }
    
    app->release();

    return 0;
}