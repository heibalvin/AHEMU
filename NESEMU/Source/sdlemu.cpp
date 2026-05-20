#include "sdlemu.hpp"
#include "nesdsk.hpp"

/*──────────────────────────────────────────────
    Constructor / Destructor
──────────────────────────────────────────────*/
SDLEMU::SDLEMU() {
    emu = new NESEMU();
    initSDL();

    previousTime = SDL_GetPerformanceCounter();
    isRunning = true;
    isUpdate = false;
}

SDLEMU::~SDLEMU() {
    isRunning = false;
    isUpdate = false;
    
    releaseSDL();

    if (emu) {
        delete emu;
        emu = NULL;
    }
}

/*──────────────────────────────────────────────
    SDL initialisation — sub-systems only.
    romName and emu dimensions are not yet known
    here, so window / renderer creation is
    deferred to initRom() (called from loadRom).
──────────────────────────────────────────────*/
void SDLEMU::initSDL() {
    if (!SDL_Init(0)) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_Init failed: %s", SDL_GetError());
        releaseSDL();
        return;
    }

    if (!SDL_CreateWindowAndRenderer("NESEMU", 1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDLinit: create window/renderer failed: %s", SDL_GetError());
        releaseSDL();
        return;
    }

    if (!SDL_SetRenderLogicalPresentation(renderer, emu->getWidth(), emu->getHeight(), SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "initRom: logical presentation failed: %s", SDL_GetError());
        releaseSDL();
        return;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, emu->getWidth(), emu->getHeight());
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDLinit: create streaming texture failed: %s", SDL_GetError());
        releaseSDL();
        return;
    }

    if (!SDL_SetRenderVSync(renderer, 1)) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDLinit: render vsync failed: %s", SDL_GetError());
        releaseSDL();
        return;
    }

    if (!SDL_ShowWindow(window)) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDLinit: show window failed: %s", SDL_GetError());
        releaseSDL();
        return;
    }

    if (!SDL_RaiseWindow(window)) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDLinit: raise window failed: %s", SDL_GetError());
        releaseSDL();
        return;
    };

    SDL_strlcpy(projectPath, SDL_GetBasePath(), sizeof(projectPath));
    getDirectoryPath(resourcePath, sizeof(resourcePath), "../Resources/");
    getDirectoryPath(outputPath, sizeof(outputPath), "../Output/");
}

void SDLEMU::releaseSDL() {
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
}

/*──────────────────────────────────────────────
    File / path helpers
──────────────────────────────────────────────*/
void SDLEMU::getDirectoryPath(char* dest, size_t destSize, const char* directory) {
    const char* basePath = SDL_GetBasePath();
    SDL_strlcpy(dest, basePath, destSize);
    SDL_strlcat(dest, directory, destSize);
}

void SDLEMU::getFilePath(char* dest, size_t destSize, const char* path, const char* filename) {
    SDL_strlcpy(dest, path, destSize);
    SDL_strlcat(dest, filename, destSize);
}

void SDLEMU::loadRom(const char* romName) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "loadRom: %s", romName);
    SDL_strlcpy(this->romName, romName, sizeof(this->romName));

    size_t romSize = 0;
    Uint8* romData = loadFile(romName, &romSize);
    if (!romData) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "loadRom failed: %s", SDL_GetError());
        return;
    }
    emu->loadRom(romData);
    SDL_free(romData);
}

SDL_Texture* SDLEMU::loadPNG(const char* filename) {
    char filepath[256];
    SDL_strlcpy(filepath, resourcePath, sizeof(filepath));
    SDL_strlcat(filepath, filename, sizeof(filepath));

    SDL_Surface* surface = SDL_LoadPNG(filepath);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "loadPNG (surface) failed: %s — %s",
                     SDL_GetError(), filename);
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "loadPNG (texture) failed: %s — %s",
                     SDL_GetError(), filename);
        SDL_DestroySurface(surface);
        return NULL;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(surface);

    return texture;
}

Uint8* SDLEMU::loadFile(const char* romname, size_t* romSize) {
    char filename[256];
    SDL_strlcpy(filename, resourcePath, sizeof(filename));
    SDL_strlcat(filename, romname, sizeof(filename));
    return (Uint8*)SDL_LoadFile(filename, romSize);
}

/*──────────────────────────────────────────────
    CHR-ROM debug export
──────────────────────────────────────────────*/
void SDLEMU::exportCHR2RGBA() {
    for (int i = 0; i < emu->dsk->chrRomCount; i++) {
        for (int offset = 0; offset < 2; offset++) {
            Uint8* rgbaData = emu->dsk->CHR2RGBA(i, offset);
            if (rgbaData) {
                char filename[128];
                SDL_snprintf(filename, sizeof(filename), "%s_chrrom_%d_%d.png", romName, i, offset);
                char filepath[256];
                getFilePath(filepath, sizeof(filepath), outputPath, filename);

                SDL_Surface* surface = SDL_CreateSurfaceFrom(128, 128,
                                            SDL_PIXELFORMAT_ABGR8888, rgbaData, 128 * 4);
                if (surface) {
                    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "CHR-ROM saved: %s", filepath);
                    SDL_SavePNG(surface, filepath);
                    SDL_DestroySurface(surface);
                }
                SDL_free(rgbaData);
            }
        }
    }
}

void SDLEMU::powerOn() {
    emu->powerOn();
}

/*──────────────────────────────────────────────
    Continuous run loop
──────────────────────────────────────────────*/
void SDLEMU::run() {
    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                isRunning = false;
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                isRunning = false;
        }

        currentTime   = SDL_GetPerformanceCounter();
        deltaTime     = (currentTime - previousTime) * 1000000000ULL / SDL_GetPerformanceFrequency();

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
    const Uint64 FRAME_NS = 1000000000ULL / 60;
    if (fps >= FRAME_NS) {
        SDL_SetRenderDrawColor(renderer, 0x70, 0x01, 0x93, 255);
        SDL_RenderClear(renderer);

        const Uint8* frameBuffer = emu->getFrameBuffer();
        if (frameBuffer && texture) {
            void*  pixels;
            int    pitch;
            SDL_LockTexture(texture, NULL, &pixels, &pitch);
            SDL_memcpy(pixels, frameBuffer, emu->getWidth() * emu->getHeight() * 4);
            SDL_UnlockTexture(texture);
            SDL_RenderTexture(renderer, texture, NULL, NULL);
        }

        SDL_RenderPresent(renderer);
        fps = 0;
    }
}

/*──────────────────────────────────────────────
    Step-by-step debugging loop (called from main)
──────────────────────────────────────────────*/
void SDLEMU::input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
                case SDLK_ESCAPE:
                    isRunning = false;
                    break;

                // CYCLE breakpoint
                case SDLK_C:
                    emu->setHaltTarget(NESEvent::CYCLE_STEP);
                    // SDL_Log("Halt Target Set: Single Master Cycle");
                    isUpdate = true;
                    break;
                // INSTRUCTION breakpoint
                case SDLK_I:
                    emu->setHaltTarget(NESEvent::INSTRUCTION_STEP);
                    // SDL_Log("Halt Target Set: Full CPU Opcode Instruction");
                    isUpdate = true;
                    break;
                // VBLANK breakpoint
                case SDLK_V:
                    emu->setHaltTarget(NESEvent::VBLANK_START);
                    // SDL_Log("Halt Target Set: V-Blank Start");
                    isUpdate = true;
                    break;
                // FRAME breakpoint
                case SDLK_F:
                    emu->setHaltTarget(NESEvent::FRAME_COMPLETE);
                    // SDL_Log("Halt Target Set: Full Frame Loop");
                    isUpdate = true;
                    emu->isContinuousRun = false;
                    break;

                // RUN without breakpoints
                case SDLK_R:
                    emu->setContinuousRun(!emu->isContinuousRun);
                    // SDL_Log(emu->isContinuousRun ? "Continuous Run: ON" : "Continuous Run: OFF");
                    break;
            }
        }
    }
}

void SDLEMU::update() {
    if (emu->isContinuousRun) {
        // Run freely without chopping on any breakpoint target
        emu->runUntilEvent();
        return;
    }

    if (!isUpdate) return;

    // Call the central runner. Components will autonomously break this loop.
    emu->runUntilEvent();

    // Log the reason the emulator paused for your debugger terminal
    switch (emu->getLastEvent()) {
        case NESEvent::INSTRUCTION_STEP:
            // Perfect place to print register traces: PC, A, X, Y
            break;
        case NESEvent::VBLANK_START:
            SDL_Log("Debugger: Stopped on V-Blank Start!");
            break;
        case NESEvent::FRAME_COMPLETE:
            // Standard frame complete pass
            SDL_Log("Debugger: Frame Complete Start!");
            break;
        default:
            break;
    }

    isUpdate = false; // Wait for the next breakpoint key press
}

void SDLEMU::render() {
    if (emu->getLastEvent() == NESEvent::FRAME_COMPLETE) {
        SDL_SetRenderDrawColor(renderer, 0x70, 0x01, 0x93, 255);
        SDL_RenderClear(renderer);

        const Uint8* frameBuffer = emu->getFrameBuffer();
        if (frameBuffer && texture) {
            void*  pixels;
            int    pitch;
            SDL_LockTexture(texture, NULL, &pixels, &pitch);
            SDL_memcpy(pixels, frameBuffer, emu->getWidth() * emu->getHeight() * 4);
            SDL_UnlockTexture(texture);
            SDL_RenderTexture(renderer, texture, NULL, NULL);
        }

        SDL_RenderPresent(renderer);
    }
}
