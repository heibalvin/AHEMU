#include "SDLEMU.hpp"

SDLEMU::SDLEMU() :
    window(nullptr),
    renderer(nullptr),
    texture(nullptr),
    isRunning(false),
    projectPath(nullptr),
    resourcePath(nullptr),
    audioStream(nullptr),       
    audioDevicePlaying(false)
{
    SDLinit();
}

SDLEMU::~SDLEMU() {
    SDLrelease();
}

bool SDLEMU::insertRom(const char* romName) {
    if (!resourcePath) {
        SDL_Log("Cartridge Insertion Error: Subsystem paths not ready.");
        return false;
    }

    char* fullRomLocationPath = nullptr;
    SDL_asprintf(&fullRomLocationPath, "%s%s", resourcePath, romName);
    if (!fullRomLocationPath) return false;

    size_t size = 0;
    void* loadedFile = SDL_LoadFile(fullRomLocationPath, &size);
    SDL_free(fullRomLocationPath);

    if (!loadedFile) {
        SDL_Log("Cartridge Insertion Failure: Media target not found inside resources: %s", romName);
        return false;
    }

    // Direct shipment to the internal CH8DSK persistent hardware block
    bool success = emu.insertRom(static_cast<Uint8*>(loadedFile), size);

    // Free transient storage immediately — CH8DSK now holds its own safe C-style block allocation
    SDL_free(loadedFile);

    if (!success) {
        SDL_Log("Cartridge Rejected: Rom dimensions violate operational specifications.");
        return false;
    }

    SDL_Log("Cartridge successfully slotted into internal persistent storage drive: %s (%zu bytes)", romName, size);
    return true;
}

void SDLEMU::powerOn() {
    // Elevate main hardware line voltages
    emu.powerOn();
    isRunning = true;
    SDL_Log("Console Main Voltage Stabilized: Core systems running.");
}

void SDLEMU::powerOff() {
    isRunning = false;
    emu.powerOff(); // Drops temporary registers, but your loaded cart remains mounted inside CH8DSK!
    SDL_Log("Console Main Power Dissipated. Storage arrays safe.");
}

void SDLEMU::run() {
    if (!isRunning) return;

    Uint64 pastPerformanceCounterTick = SDL_GetPerformanceCounter();
    const double performanceCounterFrequency = static_cast<double>(SDL_GetPerformanceFrequency());
    int lastObservedSecondValue = -1;

    while (isRunning) {
        Uint64 currentTickCount = SDL_GetPerformanceCounter();
        double elapsedDeltaTimeSeconds = static_cast<double>(currentTickCount - pastPerformanceCounterTick) / performanceCounterFrequency;
        pastPerformanceCounterTick = currentTickCount;

        if (elapsedDeltaTimeSeconds > 0.1) elapsedDeltaTimeSeconds = 0.1;

        input(); 
        emu.update(elapsedDeltaTimeSeconds);
        audio();

        if (emu.isFrameReady()) {
            render();
            emu.clearFrameReadyFlag();
        }

        if (emu.getFpsCalculated() != lastObservedSecondValue) {
            lastObservedSecondValue = emu.getFpsCalculated();
            char windowTitle[128];
            SDL_snprintf(windowTitle, sizeof(windowTitle), 
                         "CH8EMU | FPS: %d (Target: %.0f) | UPS: %d Hz (Target: %.0f Hz)", 
                         emu.getFpsCalculated(), emu.getFpsTarget(), 
                         emu.getUpsCalculated(), emu.getUpsTarget());
            SDL_SetWindowTitle(window, windowTitle);
        }
        SDL_Delay(1);
    }
}

bool SDLEMU::SDLinit() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL Initialization Failure Fault: %s", SDL_GetError());
        return false;
    }

    filesystem(); 

    window = SDL_CreateWindow("CH8EMU", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) return false;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!texture) return false;

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_AudioSpec spec;
    spec.format   = SDL_AUDIO_S16LE;  
    spec.channels = 1;                
    spec.freq     = 44100;            

    audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!audioStream) {
        SDL_Log("SDL Audio Stream Initialization Failure: %s", SDL_GetError());
    }

    return true;
}

void SDLEMU::SDLrelease() {
    // 1. Terminate visual layer subsystems safely
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    // 2. Shut down your active audio streams
    if (audioStream) {
        SDL_DestroyAudioStream(audioStream);
        audioStream = nullptr;
    }

    // 3. CLEAN UP STRINGS SAFELY BEFORE CALLING SDL_QUIT
    // Ensure you use SDL_free and set pointers to nullptr to prevent double-frees
    if (projectPath) {
        SDL_free(projectPath);
        projectPath = nullptr;
    }
    if (resourcePath) {
        SDL_free(resourcePath);
        resourcePath = nullptr;
    }

    // 4. Finally, declare global system dissipation
    SDL_Quit();
}

void SDLEMU::filesystem() {
    const char* basePath = SDL_GetBasePath();
    if (basePath) {
        // Allocate dynamic string memory that SDL understands how to track
        SDL_asprintf(&projectPath, "%s", basePath);
        SDL_asprintf(&resourcePath, "%s../Resources/", basePath);
    }
}

void SDLEMU::input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
        } 
        // Add a check to ignore repeat events completely
        else if ((event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)) {
            if (event.key.repeat) { 
                continue; // Skip OS auto-generated repeat spams
            }

            Uint8 state = (event.type == SDL_EVENT_KEY_DOWN) ? 1 : 0;
            
            switch (event.key.scancode) {
                case SDL_SCANCODE_1: emu.key.setKeyState(0x1, state); break;
                case SDL_SCANCODE_2: emu.key.setKeyState(0x2, state); break;
                case SDL_SCANCODE_3: emu.key.setKeyState(0x3, state); break;
                case SDL_SCANCODE_4: emu.key.setKeyState(0xC, state); break;
                case SDL_SCANCODE_Q: emu.key.setKeyState(0x4, state); break;
                case SDL_SCANCODE_W: emu.key.setKeyState(0x5, state); break;
                case SDL_SCANCODE_E: emu.key.setKeyState(0x6, state); break;
                case SDL_SCANCODE_R: emu.key.setKeyState(0xD, state); break;
                case SDL_SCANCODE_A: emu.key.setKeyState(0x7, state); break;
                case SDL_SCANCODE_S: emu.key.setKeyState(0x8, state); break;
                case SDL_SCANCODE_D: emu.key.setKeyState(0x9, state); break;
                case SDL_SCANCODE_F: emu.key.setKeyState(0xE, state); break;
                case SDL_SCANCODE_Z: emu.key.setKeyState(0xA, state); break;
                case SDL_SCANCODE_X: emu.key.setKeyState(0x0, state); break;
                case SDL_SCANCODE_C: emu.key.setKeyState(0xB, state); break;
                case SDL_SCANCODE_V: emu.key.setKeyState(0xF, state); break;
                default: break;
            }
        }
    }
}

void SDLEMU::update(double deltaTime) {
    emu.update(deltaTime);
}

void SDLEMU::audio() {
    if (!audioStream) return;

    if (emu.apu.isBuzzerActive()) {
        if (SDL_GetAudioStreamQueued(audioStream) < 4096) {
            const int sampleRate = 44100;
            const int targetFrequency = 440;  
            const int samplesPerHalfWave = sampleRate / (targetFrequency * 2);
            const int16_t volumeAmplitude = 3000;  
            
            static int sampleIndex = 0;
            int16_t sampleBuffer[512];

            for (int i = 0; i < 512; ++i) {
                if ((sampleIndex / samplesPerHalfWave) % 2 == 0) {
                    sampleBuffer[i] = volumeAmplitude;
                } else {
                    sampleBuffer[i] = -volumeAmplitude;
                }
                sampleIndex++;
            }
            // sampleIndex %= (sampleRate / targetFrequency);

            SDL_PutAudioStreamData(audioStream, sampleBuffer, sizeof(sampleBuffer));
        }

        if (!audioDevicePlaying) {
            SDL_ResumeAudioStreamDevice(audioStream);
            audioDevicePlaying = true;
        }
    } else {
        if (audioDevicePlaying) {
            SDL_PauseAudioStreamDevice(audioStream);
            SDL_ClearAudioStream(audioStream);  
            audioDevicePlaying = false;
        }
    }
}

void SDLEMU::render() {
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, nullptr, emu.ppu.frameBuffer, 64 * sizeof(Uint32));
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}