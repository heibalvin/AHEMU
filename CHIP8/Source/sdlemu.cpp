#include "SDLEMU.hpp"

SDLEMU::SDLEMU() :
    window(nullptr),
    renderer(nullptr),
    texture(nullptr),
    isRunning(false),
    projectPath(nullptr),
    resourcePath(nullptr),
    audioStream(nullptr),       
    audioDevicePlaying(false),
    cartridgeBuffer(nullptr),
    cartridgeSize(0)
{
    SDLinit();
}

SDLEMU::~SDLEMU() {
    // If a cartridge is still in the machine when destroyed, clear it up
    if (cartridgeBuffer) {
        SDL_free(cartridgeBuffer);
        cartridgeBuffer = nullptr;
    }
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

    // If a cartridge was already plugged in, take it out first
    if (cartridgeBuffer) {
        SDL_free(cartridgeBuffer);
        cartridgeBuffer = nullptr;
        cartridgeSize = 0;
    }

    // Read file data into local cartridge buffer slots
    void* loadedFile = SDL_LoadFile(fullRomLocationPath, &cartridgeSize);
    SDL_free(fullRomLocationPath);

    if (!loadedFile) {
        SDL_Log("Cartridge Insertion Failure: Media target not found inside resources: %s", romName);
        return false;
    }

    cartridgeBuffer = static_cast<Uint8*>(loadedFile);
    SDL_Log("Cartridge successfully slid into media deck socket: %s (%zu bytes)", romName, cartridgeSize);
    return true;
}

void SDLEMU::powerOn() {
    // 1. Physically clear system data blocks during power spike reset 
    // (This guarantees memory starts clean every time the user flips the power switch)
    emu.powerOff(); 

    if (!cartridgeBuffer || cartridgeSize == 0) {
        SDL_Log("Power On Fault: Machine powered up with an empty cartridge slot.");
        return;
    }

    // 2. Wake up target core architecture execution environments 
    emu.powerOn();

    // 3. Inject the cartridge data directly into the system bus RAM/DSK layout matrix
    if (!emu.insertRom(cartridgeBuffer, cartridgeSize)) {
        SDL_Log("Power On Fault: Insufficient system memory space to map cartridge footprint.");
        emu.powerOff();
        return;
    }

    isRunning = true;
    SDL_Log("Console Main Voltage Stabilized: Core systems running.");
}

void SDLEMU::powerOff() {
    // 1. Cut engine processing executions down
    isRunning = false;

    // 2. Physically clear internal volatile hardware registries and the DSK RAM layout spaces
    emu.powerOff(); 

    // 3. Safely eject and discard the cartridge allocation state lines
    if (cartridgeBuffer) {
        SDL_free(cartridgeBuffer);
        cartridgeBuffer = nullptr;
    }
    cartridgeSize = 0;

    SDL_Log("Console Main Power Dissipated: All storage matrices cleared out completely.");
}

void SDLEMU::run() {
    if (!isRunning) return;

    Uint64 pastPerformanceCounterTick = SDL_GetPerformanceCounter();
    const double performanceCounterFrequency = static_cast<double>(SDL_GetPerformanceFrequency());

    while (isRunning) {
        Uint64 currentTickCount = SDL_GetPerformanceCounter();
        double elapsedDeltaTimeSeconds = static_cast<double>(currentTickCount - pastPerformanceCounterTick) / performanceCounterFrequency;
        pastPerformanceCounterTick = currentTickCount;

        if (elapsedDeltaTimeSeconds > 0.1) elapsedDeltaTimeSeconds = 0.1;

        input(); 
        update(elapsedDeltaTimeSeconds);
        audio();
        render();
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
    if (audioStream) {
        SDL_DestroyAudioStream(audioStream);
        audioStream = nullptr;
    }
    audioDevicePlaying = false;

    if (projectPath)  { SDL_free(projectPath); projectPath = nullptr; }
    if (resourcePath) { SDL_free(resourcePath); resourcePath = nullptr; }

    if (texture)  { SDL_DestroyTexture(texture); texture = nullptr; }
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window); window = nullptr; }
    
    SDL_Quit();
}

void SDLEMU::filesystem() {
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

void SDLEMU::input() {
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