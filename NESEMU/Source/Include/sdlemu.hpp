#ifndef SDLEMU_HPP
#define SDLEMU_HPP

#include <SDL3/SDL.h>
#include "nesemu.hpp"

class SDLEMU {
public:
    SDLEMU();
    ~SDLEMU();

    void getDirectoryPath(char* dest, size_t destSize, const char* directory);
    void getFilePath(char* dest, size_t destSize, const char* path, const char* filename);
    
    SDL_Texture* loadPNG(const char *filename);
    Uint8* loadFile(const char *romname, size_t *romSize);
    
    void loadRom(const char* romName);
    void exportNESRomsRGBA();

    void start();
    void stop();
    void run();

    void update(Uint64 deltaTime);
    void render(Uint64 deltaTime);
    
private:
    friend class NESEMU;
    NESEMU *emu;

    // SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    bool running = false;

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