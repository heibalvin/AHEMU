#ifndef SDLEMU_HPP
#define SDLEMU_HPP

#include <SDL3/SDL.h>

class GBEMU;

class SDLEMU {
public:
    SDLEMU(const char* title, int width, int height);
    ~SDLEMU();

    bool start();
    void stop();
    void pause();
    void run();
    bool isRunning() const { return running; }

    void update();
    void render();
    void renderDebug();

private:
    // Emulator components
    GBEMU* emu;

    // SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* frameTexture = nullptr;
    bool running = false;

    // Window components
    char title[80];
    int width = 160;
    int height = 144;

    // Clock & Timer components
    Uint64 previousTime = 0.0;
    Uint64 currentTime = 0.0;
    Uint64 deltaTime = 0.0;
    
    double fps = 0.0f;
    Uint64 fpsElapse = 0;

    double ups = 0.0f;
    Uint32 upsCount = 0;
    Uint64 upsElapse = 0;
};

#endif /* SDLEMU_HPP */