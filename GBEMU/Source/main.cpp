#include <csignal>
#include <atomic>

#include "GBAPP.h"

// Global pointer for the signal handler
GBAPP* globalApp = nullptr;

void signalHandler(int signum) {
    if (globalApp) {
        // Just tell the emulator to stop. 
        // The loop in GBAPP::run() will see this change and exit automatically.
        globalApp->powerOff(); 
    }
    // We don't need to exit(signum) here if we handle the shutdown gracefully
}

int main(int argc, char* argv[]) {
    // 1. Parse command line parameters for headless execution state configurations
    bool isHeadless = false;
    for (int i = 1; i < argc; i++) {
        if (SDL_strcmp(argv[i], "--headless") == 0) {
            isHeadless = true;
            break;
        }
    }

    // 2. Instantiate your frontend application layer on stack passing configuration
    GBAPP app(isHeadless);
    globalApp = &app;
    std::signal(SIGINT, signalHandler);
    
    // 3. Fire up host windows or baseline subsystems
    app.powerOn();

    // Optional workspace loads can safely happen here:
    app.load("Resources/Tetris (JUE) (V1.1) [!].gb");

    // 4. Run the master simulation execution loop
    app.run();

    // 5. Tear down host resources cleanly
    app.powerOff();

    return 0;
}