#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "sdlemu.hpp"

int main(int argc, char *argv[]) {
    SDLEMU game("Tetris (JUE) (V1.1) [!].gb");

    game.start();
    game.run();
    game.stop();

    return 0;
}
