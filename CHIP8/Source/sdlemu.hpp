#pragma once
#include <SDL3/SDL.h>
#include "CH8EMU.hpp"

class SDLEMU {
private:
    CH8EMU emu;

    // SDL Interface Context Elements
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    bool          isRunning;

    // Platform-Independent Asset Path Tracks
    char* projectPath;
    char* resourcePath;

    SDL_AudioStream* audioStream;  
    bool audioDevicePlaying;

    // Cache the cartridge buffer tracking between insertion and power delivery
    Uint8* cartridgeBuffer;
    size_t cartridgeSize;

public:
    SDLEMU();   // Constructor: Maps SDLinit() only
    ~SDLEMU();  // Destructor: Maps SDLrelease() only

    SDLEMU(const SDLEMU&) = delete;
    SDLEMU& operator=(const SDLEMU&) = delete;

    // Physical Media & Power Subsystem Interface Matrix
    bool insertRom(const char* romName); 
    void powerOn();                      
    void run();                          
    void powerOff();                     

private:
    const int WINDOW_WIDTH  = 1280;
    const int WINDOW_HEIGHT = 640;

    bool SDLinit();     
    void SDLrelease();  

    void filesystem();  
    void input();       
    void update(double deltaTime);
    void audio();
    void render();
};