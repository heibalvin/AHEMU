#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "sdlemu.hpp"

int main(int argc, char *argv[]) {
    SDLEMU game("GBEMU", 160, 144);

    game.start();
    game.run();
    game.stop();

    return 0;
}
