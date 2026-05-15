#ifndef SDLEMU_HPP
#define SDLEMU_HPP

#include <SDL3/SDL.h>
#include "gbemu.hpp"

class SDLEMU {
public:
    SDLEMU(const char* romName, int width, int height);
    ~SDLEMU();

    SDL_Texture* loadPNG(const char *filename);
    Uint8* loadFile(const char *romname, size_t *romSize);
    
    bool start();
    void stop();
    void run();

    void update(double deltaTime);
    void render(double deltaTime);
    

private:
    GBEMU *emu;

    // SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    bool running = false;

    // File path components
    char projectPath[256];
    char resourcePath[256];
    char romName[128];

    // Window components
    int width = 160;
    int height = 144;

    // Clock & Timer components
    double performance_frequency;
    Uint64 previousTime = 0.0;
    Uint64 currentTime = 0.0;
};

#endif /* SDLEMU_HPP */