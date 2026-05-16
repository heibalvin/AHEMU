#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "sdlemu.hpp"

int main(int argc, char *argv[]) {
    SDLEMU game;

    game.loadRom("Xevious (Europe).nes");
    game.exportCHR2RGBA();

    game.start();
    game.run();

    return 0;
}
