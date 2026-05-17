#ifndef SDLEMU_HPP
#define SDLEMU_HPP

#include <SDL3/SDL.h>
#include "nesemu.hpp"

class SDLEMU {
public:
    SDLEMU();
    ~SDLEMU();

    // SDL component management
    void initSDL();
    void releaseSDL();

    // File management
    void getDirectoryPath(char* dest, size_t destSize, const char* directory);
    void getFilePath(char* dest, size_t destSize, const char* path, const char* filename);
    SDL_Texture* loadPNG(const char *filename);
    Uint8* loadFile(const char *romname, size_t *romSize);
    
    // Rom management
    void loadRom(const char* romName);
    void exportCHR2RGBA();

    // Application LifeCycle
    bool isRunning = false;
    void powerOn();
    void run();
    void update(Uint64 deltaTime);
    void render(Uint64 deltaTime);

    // Application LifeCycle - for debugging step by step
    bool isUpdate = false;
    void input();
    void update();
    void render();
    
private:
    friend class NESEMU;
    NESEMU *emu;

    // SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    // File path components
    char projectPath[256];
    char resourcePath[256];
    char outputPath[256];
    char romName[128];

    // Clock & Timer components
    Uint64 previousTime = 0.0;
    Uint64 currentTime = 0.0;
    Uint64 deltaTime = 0.0;
    Uint64 fps = 0.0;
};

#endif /* SDLEMU_HPP */