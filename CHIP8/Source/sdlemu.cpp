#include "SDLEMU.hpp"

SDLEMU::SDLEMU() :
    window(nullptr),
    renderer(nullptr),
    texture(nullptr),
    isRunning(false),
    projectPath(nullptr),
    resourcePath(nullptr),
    audioDevice(0),
    audioDevicePlaying(false)
{}

SDLEMU::~SDLEMU() {
    release();
}

/**
 * Evaluates the execution environment using SDL3 string utility pipelines
 * to safely support vanilla macOS directories and Xcode app bundles.
 */
void SDLEMU::resolvePlatformPaths() {
    const char* basePathStr = SDL_GetBasePath();
    if (!basePathStr) {
        projectPath = SDL_strdup("./");
        resourcePath = SDL_strdup("./Resources/");
        return;
    }

    if (SDL_strstr(basePathStr, ".app/Contents/Resources/") != nullptr ||
        SDL_strstr(basePathStr, ".app/Contents/MacOS/") != nullptr) {
        projectPath = SDL_strdup(basePathStr);
    } else {
        SDL_asprintf(&projectPath, "%s../", basePathStr);
    }

    SDL_asprintf(&resourcePath, "%sResources/", projectPath);
    SDL_Log("Platform paths resolved -> Project Root: %s | Resources: %s", projectPath, resourcePath);
    SDL_free((char *)basePathStr); 
}

/**
 * Initializes physical audio device descriptors and loops a standard square wave beep stream.
 */
void SDLEMU::initAudioSubsystem() {
    SDL_AudioSpec spec;
    spec.format   = SDL_AUDIO_S16LE;
    spec.channels = 1;
    spec.freq     = 44100;

    audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
}

bool SDLEMU::init() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL Initialization Failure Fault: %s", SDL_GetError());
        return false;
    }

    resolvePlatformPaths();

    window = SDL_CreateWindow("CHIP-8 Virtual Framework Core", 64 * 12, 32 * 12, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) return false;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!texture) return false;

    initAudioSubsystem();

    emu.powerOn();
    isRunning = true;
    return true;
}

void SDLEMU::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
        } 
        else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            Uint8 state = (event.type == SDL_EVENT_KEY_DOWN) ? 1 : 0;
            
            switch (event.key.scancode) {
                case SDL_SCANCODE_1: emu.con.setKeyState(0x1, state); break;
                case SDL_SCANCODE_2: emu.con.setKeyState(0x2, state); break;
                case SDL_SCANCODE_3: emu.con.setKeyState(0x3, state); break;
                case SDL_SCANCODE_4: emu.con.setKeyState(0xC, state); break;
                case SDL_SCANCODE_Q: emu.con.setKeyState(0x4, state); break;
                case SDL_SCANCODE_W: emu.con.setKeyState(0x5, state); break;
                case SDL_SCANCODE_E: emu.con.setKeyState(0x6, state); break;
                case SDL_SCANCODE_R: emu.con.setKeyState(0xD, state); break;
                case SDL_SCANCODE_A: emu.con.setKeyState(0x7, state); break;
                case SDL_SCANCODE_S: emu.con.setKeyState(0x8, state); break;
                case SDL_SCANCODE_D: emu.con.setKeyState(0x9, state); break;
                case SDL_SCANCODE_F: emu.con.setKeyState(0xE, state); break;
                case SDL_SCANCODE_Z: emu.con.setKeyState(0xA, state); break;
                case SDL_SCANCODE_X: emu.con.setKeyState(0x0, state); break;
                case SDL_SCANCODE_C: emu.con.setKeyState(0xB, state); break;
                case SDL_SCANCODE_V: emu.con.setKeyState(0xF, state); break;

                case SDL_SCANCODE_F10:
                    if (state == 1) { CH8_DBG_EXEC(emu.dbg.stepInstruction()); }
                    break;
                case SDL_SCANCODE_F5:
                    if (state == 1) { CH8_DBG_EXEC(emu.dbg.resumeExecution()); }
                    break;
                case SDL_SCANCODE_F6:
                    if (state == 1) { CH8_DBG_EXEC(emu.dbg.pauseExecution()); }
                    break;
                default: break;
            }
        }
    }
}

/**
 * ── Stage A: Core Synchronization Update ──
 * Directs processing time slicing increments into the hardware emulator backend.
 */
void SDLEMU::update(double deltaTime) {
    emu.update(deltaTime);
}

/**
 * ── Stage B: Platform Audio Control ──
 * Checks the abstract state in the APU to adjust physical hardware playback.
 */
void SDLEMU::audio() {
    if (audioDevice == 0) return;

    if (emu.apu.isBuzzerActive()) {
        if (!audioDevicePlaying) {
            SDL_ResumeAudioDevice(audioDevice);
            audioDevicePlaying = true;
        }
    } else {
        if (audioDevicePlaying) {
            SDL_PauseAudioDevice(audioDevice);
            audioDevicePlaying = false;
        }
    }
}

/**
 * ── Stage C: Graphics Presentation ──
 * Locks the backbuffer and uploads textures directly into the GPU pipeline frame.
 */
void SDLEMU::render() {
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, nullptr, emu.ppu.frameBuffer, 64 * sizeof(Uint32));
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void SDLEMU::run(const char* romName) {
    const char* searchPaths[] = { resourcePath, "./Resources/", "./" };
    void* romBuffer = nullptr;
    size_t fileSize = 0;
    char* fullRomLocationPath = nullptr;

    for (int i = 0; i < 3 && !romBuffer; ++i) {
        SDL_asprintf(&fullRomLocationPath, "%s%s", searchPaths[i], romName);
        romBuffer = SDL_LoadFile(fullRomLocationPath, &fileSize);
        if (fullRomLocationPath) SDL_free(fullRomLocationPath);
        fullRomLocationPath = nullptr;
    }

    if (!romBuffer) {
        SDL_Log("ROM Ingestion Failure: Unable to open path target: %s", romName);
        return;
    } 

    if (!emu.injectROM(static_cast<const Uint8*>(romBuffer), fileSize)) {
        SDL_Log("ROM Load Core Error: Storage boundary capacity exception.");
        SDL_free(romBuffer);
        return;
    }
    SDL_free(romBuffer);

    Uint64 pastPerformanceCounterTick = SDL_GetPerformanceCounter();
    const double performanceCounterFrequency = static_cast<double>(SDL_GetPerformanceFrequency());

    // --- Master Fixed Pipeline Loop ---
    while (isRunning) {
        Uint64 currentTickCount = SDL_GetPerformanceCounter();
        double elapsedDeltaTimeSeconds = static_cast<double>(currentTickCount - pastPerformanceCounterTick) / performanceCounterFrequency;
        pastPerformanceCounterTick = currentTickCount;

        if (elapsedDeltaTimeSeconds > 0.1) elapsedDeltaTimeSeconds = 0.1;

        processInput();
        update(elapsedDeltaTimeSeconds);
        audio();
        render();
    }
}

void SDLEMU::release() {
    emu.powerOff();

    if (audioDevice > 0) {
        SDL_CloseAudioDevice(audioDevice);
        audioDevice = 0;
    }
    audioDevicePlaying = false;

    if (projectPath)  { SDL_free(projectPath); projectPath = nullptr; }
    if (resourcePath) { SDL_free(resourcePath); resourcePath = nullptr; }

    if (texture)  { SDL_DestroyTexture(texture); texture = nullptr; }
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window); window = nullptr; }
    
    SDL_Quit();
}