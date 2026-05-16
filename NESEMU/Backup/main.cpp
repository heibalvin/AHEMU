#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "nesemu.hpp"
#include <cstdio>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *frameTexture = NULL;
static NESEMU *emu = nullptr;

int main(int argc, char *argv[]) {
    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // 2. Create Window & Renderer
    if (!SDL_CreateWindowAndRenderer("NESEMU", 256, 240, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return 1;
    }

    // 3. Create texture for frame buffer (RGBA32, streaming)
    frameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    if (!frameTexture) {
        SDL_Log("Couldn't create texture: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 4. Initialize NESEMU
    emu = new NESEMU();

    // 5. Main Loop
    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        const double currentTime = (double)SDL_GetTicksNS() / 1e9;
        emu->update(currentTime);

        if (emu->isRefreshRequested()) {
            // Update texture with new frame data
            const auto& fb = emu->getFrameBuffer();
            SDL_UpdateTexture(frameTexture, NULL, fb.data(), 256 * 4);

            // Render frame
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, frameTexture, NULL, NULL);
            SDL_RenderPresent(renderer);

            emu->clearRefreshRequest();
        }
    }

    // 6. Cleanup
    SDL_DestroyTexture(frameTexture);
    delete emu;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}