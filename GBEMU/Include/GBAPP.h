#ifndef GBAPP_H
#define GBAPP_H

#include <SDL3/SDL.h>
#include "GBEMU.h"

class GBAPP {
private:
    GBEMU           emu;
    bool            isHeadless;

    SDL_Window*     window;
    SDL_Renderer*   renderer;
    SDL_Texture*    texture;

public:
    explicit GBAPP(bool isHeadless = false);
    ~GBAPP();

    void powerOn();
    void powerOff();
    void reset();

    void step();
    void run();

    void load(const char* filepath);
};

#endif // GBAPP_H