#include "sdlemu.hpp"

SDLEMU::SDLEMU(const char* romName, int width, int height)
    : width(width), height(height) {
    emu = new GBEMU();

    const char* path = SDL_GetBasePath();
    SDL_strlcpy(projectPath, path, sizeof(projectPath));
    
    SDL_strlcpy(resourcePath, projectPath, sizeof(resourcePath));
    SDL_strlcat(resourcePath, "../Resources/", sizeof(resourcePath));
    SDL_strlcpy(this->romName, romName, sizeof(this->romName));

    // SDL_Log("SDLEMU: project path: %s", projectPath);
    // SDL_Log("SDLEMU: resource path: %s", resourcePath);
    // SDL_Log("SDLEMU: ROM name: %s", this->romName);
}

SDLEMU::~SDLEMU() {
    stop();

    if (emu) {
        delete emu;
        emu = NULL;
    }
}

SDL_Texture* SDLEMU::loadPNG(const char *filename) {
    char filepath[256];
    SDL_strlcpy(filepath, resourcePath, sizeof(filepath));
    SDL_strlcat(filepath, filename, sizeof(filepath));

    SDL_Surface *surface = SDL_LoadPNG(filepath);
    if (!surface) {
        SDL_Log("SDLEMU: loading PNG as Surface failed: %s", SDL_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("SDLEMU: loading PNG as Texture failed: %s", SDL_GetError());
        return NULL;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_DestroySurface(surface);
    surface = NULL;

    return texture;
}

Uint8* SDLEMU::loadFile(const char *romname, size_t *romSize) {
    char filename[256];
    SDL_strlcpy(filename, resourcePath, sizeof(filename));
    SDL_strlcat(filename, romname, sizeof(filename));

    Uint8 *romData = (Uint8*)SDL_LoadFile(filename, romSize);
    return romData;
}

bool SDLEMU::start() {
    // Initialize SDL Subsystems
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        stop();
        return false;
    }

    // Create the Window and Renderer
    if (!SDL_CreateWindowAndRenderer(romName, 1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("SDLEMU: create window and renderer failed: %s", SDL_GetError());
        stop();
        return false;
    }

    // Set the Logical Presentation Size
    if (!SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        SDL_Log("SDLEMU: render logical presentation failed: %s", SDL_GetError());
        stop();
        return false;
    }

    size_t romSize = 0;
    const Uint8 *romData = loadFile(romName, &romSize);
    if (!romData) {
        SDL_Log("SDLEMU: load ROM failed: %s", SDL_GetError());
        stop();
        return false;
    }
    emu->loadRom(romData, romSize);
    SDL_free((void*)romData);

    texture = loadPNG("tetris_gameboy_00.png");
    if (!texture) {
        SDL_Log("SDLEMU: load PNG failed: %s", SDL_GetError());
        stop();
        return false;
    }

    // Make it visible
    SDL_ShowWindow(window);

    // Bring it to the top
    SDL_RaiseWindow(window); 
    
    previousTime = SDL_GetTicksNS();
    running = true;
    return true;
}

void SDLEMU::stop() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();
    running = false;
}

void SDLEMU::run() {
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        currentTime = SDL_GetTicksNS();
        deltaTime = currentTime - previousTime;

        update(deltaTime);
        render(deltaTime);

        previousTime = currentTime;
    }
}

void SDLEMU::update(Uint64 deltaTime) {
    // emu->update(deltaTime);
}

void SDLEMU::render(Uint64 deltaTime) {
     // Clear with black
     SDL_SetRenderDrawColor(renderer, 0x70, 0x01, 0x93, 255);
     SDL_RenderClear(renderer);

      // Render the loaded image
      if (texture) {
        SDL_RenderTexture(renderer, texture, NULL, NULL);
      }

     SDL_RenderPresent(renderer);
}

