#include "GBAPP.h"

GBAPP::GBAPP(bool isHeadless) 
    : window(nullptr), renderer(nullptr), texture(nullptr), isHeadless(isHeadless) {

	}

GBAPP::~GBAPP() {
	
}

void GBAPP::powerOn() {
    if (isHeadless) {        
        // Initialize SDL with NO subsystems for deterministic unit testing or terminal runs
        if (!SDL_Init(0)) {
            SDL_Log("APP: SDL Init Failure: %s", SDL_GetError());
            return;
        }
    } else {        
        // Full initialization for standard interactive display window mode
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
            SDL_Log("APP: SDL Audio, Video, Events Init Failure: %s", SDL_GetError());
            return;
        }

        window = SDL_CreateWindow("GameBoy Emulator", 800, 600, 0);
        if (!window) return;

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) return;

		SDL_SetRenderLogicalPresentation(renderer, emu.width, emu.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

		SDL_SetRenderDrawColor(renderer, 0x08, 0x18, 0x20, 0xFF); // gray
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
    }

	emu.powerOn();
}

void GBAPP::powerOff() {
    if (!emu.isRunning) {
        return;
    }
    
	emu.powerOff();

	if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);     window = nullptr; }
	if (texture)  { SDL_DestroyTexture(texture);   texture = nullptr; }

    SDL_Quit();
}

void GBAPP::reset() {
    emu.reset();
}

void GBAPP::step() {
    emu.step();
}

void GBAPP::run() {
    if (isHeadless) {
        while (emu.isRunning) {
            step();
        }
        return;
    }

    SDL_Event event;
    while (emu.isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                emu.isRunning = false;
            }
            // Add keyboard mapping here
            else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
                bool pressed = (event.type == SDL_EVENT_KEY_DOWN);
                switch (event.key.key) {
                    case SDLK_Z:     emu.joy.setButton(GBJOY::A,      pressed); break;
                    case SDLK_X:     emu.joy.setButton(GBJOY::B,      pressed); break;
                    case SDLK_RETURN:emu.joy.setButton(GBJOY::START,  pressed); break;
                    case SDLK_RSHIFT:emu.joy.setButton(GBJOY::SELECT, pressed); break;
                    case SDLK_UP:    emu.joy.setButton(GBJOY::UP,     pressed); break;
                    case SDLK_DOWN:  emu.joy.setButton(GBJOY::DOWN,   pressed); break;
                    case SDLK_LEFT:  emu.joy.setButton(GBJOY::LEFT,   pressed); break;
                    case SDLK_RIGHT: emu.joy.setButton(GBJOY::RIGHT,  pressed); break;
                }
            }
        }

        step();

        SDL_SetRenderDrawColor(renderer, 0x08, 0x18, 0x20, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
}

// ======================================================================
// PURE FRONTEND RAW ARRAY TRANSLATORS (Driven via SDL_LoadFile)
// ======================================================================
void GBAPP::load(const char* filepath) {
    size_t fileSize = 0;
    void* rawBuffer = SDL_LoadFile(filepath, &fileSize);
    if (!rawBuffer) {
        SDL_Log("APP: Failed to load ROM file '%s': %s", filepath, SDL_GetError());
        return;
    }

    // Delegate the injection to the core
    emu.load(static_cast<const Uint8*>(rawBuffer), fileSize);
    SDL_free(rawBuffer);
}
