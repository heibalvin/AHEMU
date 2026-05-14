#include "sdlemu.hpp"
#include "gbemu.hpp"

SDLEMU::SDLEMU(const char* title, int width, int height)
    : emu(nullptr), width(width), height(height) {
    SDL_strlcpy(this->title, title, sizeof(this->title));
}

SDLEMU::~SDLEMU() {
    stop();
}

bool SDLEMU::start() {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer(title, width, height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("SDLEMU: create window and renderer failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    if (!SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        SDL_Log("SDLEMU: render logical presentation failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    frameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!frameTexture) {
        SDL_Log("SDLEMU: create frameTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    emu = new GBEMU();
    emu->start();
    running = true;

    previousTime = SDL_GetTicksNS();
    return true;
}

void SDLEMU::stop() {
    if (emu) {
        emu->stop();
        delete emu;
        emu = nullptr;
    }
    if (frameTexture) {
        SDL_DestroyTexture(frameTexture);
        frameTexture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    running = false;
}

void SDLEMU::pause() {
    running = false;
}

void SDLEMU::update() {
    emu->update(deltaTime);
    upsCount++;
    SDL_Log("SDLEMU: Update()");
}

void SDLEMU::render() {
    if (emu && emu->isRefreshRequested()) {
        const auto& fb = emu->getFrameBuffer();
        SDL_UpdateTexture(frameTexture, NULL, fb.data(), width * 4);

        SDL_RenderClear(renderer);
        // SDL_RenderTexture(renderer, frameTexture, NULL, NULL);
        
        // Debug
        char buf[32];

        fps = 1 / (double)fpsElapse / SDL_NS_PER_SECOND;
        snprintf(buf, sizeof(buf), "FPS: %.1f Hz", fps);
        SDL_RenderDebugText(renderer, 5, 5, buf);
        fpsElapse = 0.0;

        ups = (double)upsCount / (double)upsElapse / SDL_NS_PER_SECOND;
        snprintf(buf, sizeof(buf), "UPS: %.1f Mz", ups / 1000000.0);
        SDL_RenderDebugText(renderer, 5, 20, buf);

        SDL_RenderPresent(renderer);

        emu->clearRefreshRequest();

        SDL_Log("SDLEMU: Render()");
    }
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
        
        update();
        render();

        fpsElapse += deltaTime;
        upsElapse += deltaTime;
        previousTime = currentTime;
    }
}