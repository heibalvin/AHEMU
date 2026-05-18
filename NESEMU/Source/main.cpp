#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "sdlemu.hpp"

int main(int argc, char *argv[]) {
    SDLEMU game;

    game.loadRom("1942 (Japan, USA).nes");
    game.powerOn();

    while (game.isRunning) {
        game.input();
        if (game.isUpdate) {
            game.update();
        }
        game.render();
    }

    return 0;
}
