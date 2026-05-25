#pragma once
#include <SDL3/SDL.h>
#include "CH8EMU.hpp"

class SDLEMU {
private:
    // Core engine orchestration instance 
    CH8EMU emu;

    // SDL3 Interface Context Elements
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    bool          isRunning;

    // Platform-Independent Asset Path Tracks (Completely STL-Free)
    char* projectPath;
    char* resourcePath;

    // Change these fields inside your SDLEMU class definition:
    SDL_AudioStream* audioStream;  // Replaces SDL_AudioDeviceID audioDevice;
    bool audioDevicePlaying;

public:
    SDLEMU();
    ~SDLEMU();

    bool init();
    void run(const char* romName);
    void release();

private:
    void resolvePlatformPaths();
    void initAudioSubsystem();
    void processInput();
    
    // Dedicated Pipeline Stages called by the master running loop
    void update(double deltaTime);
    void audio();
    void render();
};