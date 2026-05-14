#include "sdlemu.hpp"

int main(int argc, char *argv[]) {
    SDLEMU sdl("GBEMU", 800, 600);

    if (!sdl.start()) {
        return 1;
    }

    sdl.run();
    sdl.stop();

    return 0;
}