#include "sdlemu.hpp"

SDLEMU::SDLEMU(const char* romName) {
    emu = new NESEMU();
    
    const char* path = SDL_GetBasePath();
    SDL_strlcpy(projectPath, path, sizeof(projectPath));
    
    SDL_strlcpy(resourcePath, projectPath, sizeof(resourcePath));
    SDL_strlcat(resourcePath, "../Resources/", sizeof(resourcePath));
    SDL_strlcpy(this->romName, romName, sizeof(this->romName));

    previousTime = SDL_GetPerformanceCounter();
}

SDLEMU::~SDLEMU() {
    stop();

    if (emu) {
        delete emu;
        emu = NULL;
    }
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
    if (!SDL_SetRenderLogicalPresentation(renderer, emu->getWidth(), emu->getHeight(), SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        SDL_Log("SDLEMU: render logical presentation failed: %s", SDL_GetError());
        stop();
        return false;
    }

    size_t romSize = 0;
    Uint8 *romData = loadFile(romName, &romSize);
    if (!romData) {
        SDL_Log("SDLEMU: load ROM failed: %s", SDL_GetError());
        stop();
        return false;
    }
    emu->loadRom(romData, romSize);
    SDL_free((void*)romData);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, emu->getWidth(), emu->getHeight());
    if (!texture) {
        SDL_Log("SDLEMU: create texture failed: %s", SDL_GetError());
        stop();
        return false;
    }

    /* Debug Tetris Image
    texture = loadPNG("tetris_gameboy_00.png");
    if (!texture) {
        SDL_Log("SDLEMU: load PNG failed: %s", SDL_GetError());
        stop();
        return false;
    } 
    */

    // Make it visible
    SDL_ShowWindow(window);

    // Bring it to the top
    SDL_RaiseWindow(window); 
    
    previousTime = SDL_GetPerformanceCounter();
    running = true;

    // Set initial window title
    SDL_SetWindowTitle(window, romName);
    
    return true;
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

currentTime = SDL_GetPerformanceCounter();
        deltaTime = (currentTime - previousTime) * 1000000000ULL / SDL_GetPerformanceFrequency();

        update(deltaTime);
        render(deltaTime);
        
        previousTime = currentTime;
    }
}

void SDLEMU::update(Uint64 deltaTime) {
    emu->update(deltaTime);
}

void SDLEMU::render(Uint64 deltaTime) {
    fps += deltaTime;
    const Uint64 FRAME_NS = 1000000000ULL / 60; // ~60 Hz (16,666,667 ns per frame)
    if (fps >= FRAME_NS) { // update every 1/60th of a second
        SDL_SetRenderDrawColor(renderer, 0x70, 0x01, 0x93, 255);
        SDL_RenderClear(renderer);

        /* Debug: Tetris Image
        if (texture) {
            SDL_RenderTexture(renderer, texture, NULL, NULL);
        } 
        */

        const Uint8 *frameBuffer = emu->getFrameBuffer();
        if (frameBuffer && texture) {
            void* pixels;
            int pitch;
            SDL_LockTexture(texture, NULL, &pixels, &pitch);
            SDL_memcpy(pixels, frameBuffer, emu->getWidth() * emu->getHeight() * 4);
            SDL_UnlockTexture(texture);

            SDL_RenderTexture(renderer, texture, NULL, NULL);
        }

        SDL_RenderPresent(renderer);
        fps = 0;
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
