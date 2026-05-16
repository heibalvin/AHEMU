#ifndef SDLEMU_HPP
#define SDLEMU_HPP

#include <SDL3/SDL.h>
#include "nesemu.hpp"

class SDLEMU {
public:
    SDLEMU(const char* romName);
    ~SDLEMU();

    SDL_Texture* loadPNG(const char *filename);
    Uint8* loadFile(const char *romname, size_t *romSize);
    
    bool start();
    void stop();
    void run();

    void update(Uint64 deltaTime);
    void render(Uint64 deltaTime);
    

private:
    NESEMU *emu;

    // SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    bool running = false;

    // File path components
    char projectPath[256];
    char resourcePath[256];
    char romName[128];

    // Clock & Timer components
    Uint64 previousTime = 0.0;
    Uint64 currentTime = 0.0;
    Uint64 deltaTime = 0.0;
    Uint64 fps = 0.0;
};

#endif /* SDLEMU_HPP */