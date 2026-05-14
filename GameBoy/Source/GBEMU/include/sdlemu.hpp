#ifndef SDLEMU_HPP
#define SDLEMU_HPP

#include <SDL3/SDL.h>
#include <png.h>

class GBEMU;

class SDLEMU {
public:
    SDLEMU(const char* title, int width, int height);
    ~SDLEMU();

    bool start();
    void stop();
    void run();

    void update();
    void render();

private:
    // SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    bool running = false;

    // File path components
    char projectPath[256];
    char resourcePath[256];

    // Window components
    char title[80];
    int width = 160;
    int height = 144;

    // Clock & Timer components
    Uint64 previousTime = 0.0;
    Uint64 currentTime = 0.0;
    Uint64 deltaTime = 0.0;
};

#endif /* SDLEMU_HPP */