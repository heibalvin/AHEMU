#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <sstream>

// --- NES Hardware Constants ---
const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 240;
const double NES_NTSC_FPS = 60.0988;
const double TIME_PER_FRAME = 1.0 / NES_NTSC_FPS;

// Hardware timing dividers
const int CPU_CLOCK_DIVIDER = 3; // 3 PPU cycles = 1 CPU cycle
const int PPU_CYCLES_PER_FRAME = 89342; // 341 dots * 262 scanlines

// --- Embedded Metrics Trackers with MHz Conversions ---
struct CPUMetrics {
    uint64_t total_cycles = 0;
    double current_ups_mhz = 0.0;
    uint32_t counter = 0;

    void Tick() {
        total_cycles++;
        counter++;
    }

    void UpdateMetrics() {
        // Convert raw cycle count to MegaHertz (cycles per second / 1,000,000)
        current_ups_mhz = static_cast<double>(counter) / 1000000.0;
        counter = 0;
    }
};

struct PPUMetrics {
    uint64_t total_cycles = 0;
    double current_ups_mhz = 0.0;
    uint32_t current_fps = 0;
    uint32_t cycle_counter = 0;
    uint32_t frame_counter = 0;

    void TickCycle() {
        total_cycles++;
        cycle_counter++;
    }

    void TickFrame() {
        frame_counter++;
    }

    void UpdateMetrics() {
        // Convert raw clock cycles to MegaHertz
        current_ups_mhz = static_cast<double>(cycle_counter) / 1000000.0;
        current_fps = frame_counter;
        cycle_counter = 0;
        frame_counter = 0;
    }
};

// --- Emulated Core Modules ---
struct MockCPU {
    CPUMetrics metrics;

    void Step() {
        metrics.Tick();
    }
};

struct MockPPU {
    PPUMetrics metrics;
    int current_dot = 0;
    int current_scanline = 0;
    std::vector<uint32_t> pixel_buffer;

    MockPPU() : pixel_buffer(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000) {}

    bool Step() {
        metrics.TickCycle();
        bool frame_ready = false;

        if (current_scanline < SCREEN_HEIGHT && current_dot < SCREEN_WIDTH) {
            int index = current_scanline * SCREEN_WIDTH + current_dot;
            
            uint8_t r = static_cast<uint8_t>((current_dot + current_scanline) & 0xFF);
            uint8_t g = static_cast<uint8_t>(current_dot & 0xFF);
            uint8_t b = static_cast<uint8_t>(rand() % 255);
            
            pixel_buffer[index] = (0xFF << 24) | (r << 16) | (g << 8) | b;
        }

        current_dot++;
        if (current_dot >= 341) {
            current_dot = 0;
            current_scanline++;
            
            if (current_scanline >= 262) {
                current_scanline = 0;
                frame_ready = true; 
                metrics.TickFrame(); 
            }
        }
        return frame_ready;
    }
};

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "SDL3 Init Failure: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("NES Dual-Core Engine (MHz Tracking)", SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    MockCPU cpu;
    MockPPU ppu;
    
    uint64_t cycleId = 0; 
    bool is_running = true;
    SDL_Event event;

    Uint64 performance_frequency = SDL_GetPerformanceFrequency();
    Uint64 last_time = SDL_GetPerformanceCounter();
    Uint64 metric_timer = last_time;
    double time_accumulator = 0.0;

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) is_running = false;
            }
        }

        Uint64 current_time = SDL_GetPerformanceCounter();
        double delta_time = static_cast<double>(current_time - last_time) / performance_frequency;
        last_time = current_time;
        time_accumulator += delta_time;

        if (time_accumulator > 0.1) time_accumulator = 0.1;

        while (time_accumulator >= TIME_PER_FRAME) {
            bool frame_complete = false;
            int cycles_this_frame = 0;

            while (!frame_complete && cycles_this_frame < PPU_CYCLES_PER_FRAME * 2) {
                frame_complete = ppu.Step();
                
                if (cycleId % CPU_CLOCK_DIVIDER == 0) {
                    cpu.Step();
                }

                cycleId++;
                cycles_this_frame++;
            }

            time_accumulator -= TIME_PER_FRAME;

            if (frame_complete) {
                SDL_UpdateTexture(texture, nullptr, ppu.pixel_buffer.data(), SCREEN_WIDTH * sizeof(uint32_t));
                SDL_RenderClear(renderer);
                SDL_RenderTexture(renderer, texture, nullptr, nullptr);
                SDL_RenderPresent(renderer);
            }
        }

        // --- Metric Serialization & Formatting ---
        if (current_time - metric_timer >= performance_frequency) {
            cpu.metrics.UpdateMetrics();
            ppu.metrics.UpdateMetrics();

            // Set stream precision format to cleanly present sub-decimal MHz values
            std::stringstream title_stream;
            title_stream << std::fixed << std::setprecision(4)
                         << "System | CPU: " << cpu.metrics.current_ups_mhz << " MHz"
                         << " | PPU: " << ppu.metrics.current_ups_mhz << " MHz"
                         << " | Screen: " << ppu.metrics.current_fps << " FPS";
            
            SDL_SetWindowTitle(window, title_stream.str().c_str());
            metric_timer = current_time;
        }

        SDL_Delay(1); 
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
