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

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
                                    SDL_TEXTUREACCESS_STREAMING, 160, 144);

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

void GBAPP::run() {
    if (isHeadless) {
        while (emu.isRunning) {
            emu.step();
        }
        return;
    }

    Uint64 lastTime = SDL_GetTicksNS();
    const Uint64 frameTimeNs = 1000000000 / 60; // 60Hz target

    while (emu.isRunning) {
        inputs(); // Handle key events
        
        Uint64 currentTime = SDL_GetTicksNS();
        Uint64 deltaTime = currentTime - lastTime;

        // Run the emulator for the elapsed time
        emu.run(deltaTime);
        lastTime = currentTime;

        render(); // Update texture from VDP buffer

        // Simple frame rate cap (optional: allow emulator to catch up)
        if (deltaTime < frameTimeNs) {
            SDL_DelayNS(frameTimeNs - deltaTime);
        }
    }
}
void GBAPP::inputs() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) emu.isRunning = false;
        
        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            bool pressed = (event.type == SDL_EVENT_KEY_DOWN);
            switch (event.key.key) {
                case SDLK_Z:      emu.setJoypadButton(GBJOY::A, pressed);      break;
                case SDLK_X:      emu.setJoypadButton(GBJOY::B, pressed);      break;
                case SDLK_RETURN: emu.setJoypadButton(GBJOY::START, pressed);  break;
                case SDLK_RSHIFT: emu.setJoypadButton(GBJOY::SELECT, pressed); break;
                case SDLK_UP:     emu.setJoypadButton(GBJOY::UP, pressed);     break;
                case SDLK_DOWN:   emu.setJoypadButton(GBJOY::DOWN, pressed);   break;
                case SDLK_LEFT:   emu.setJoypadButton(GBJOY::LEFT, pressed);   break;
                case SDLK_RIGHT:  emu.setJoypadButton(GBJOY::RIGHT, pressed);  break;
                default: break;
            }
        }
    }
}

void GBAPP::render() {
    if (!texture) {
        return;
    }

    // 2. Update from VDP buffer
    const Uint32* pixels = emu.getVDPFrameBuffer();
    SDL_UpdateTexture(texture, nullptr, pixels, 160 * sizeof(Uint32));
    
    // 3. Draw
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
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
