#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "sdlemu.hpp"

int main(int argc, char *argv[]) {
    SDLEMU game("1942 (Japan, USA).nes");

    game.start();
    game.run();

    return 0;
}
