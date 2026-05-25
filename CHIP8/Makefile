# ─ CHIP-8 Emulator ──────────────────────────────────────────────
#  SDL3 for window, renderer, events, logging, and portable types.
#
#  Class hierarchy:
#    CH8COM        — abstract base: powerOn / powerOff / reset / step
#    CH8DSK        — ROM / disk,  SDL_IOFromFile, SDL_malloc/SDL_free
#    CH8BUS        — 4 KB RAM,   read/write, SDL_memset
#    CH8CPU        — registers, opcode decoder, delay+st timers
#    CH8PPU        — 64×32 pixel buffer, SDL_Texture upload
#    CH8APU        — square-wave audio on SDL3 audio device
#    CH8CON        — 16-key keypad, SDL scancode mapping
#    CH8EMU        — orchestrator: owns all components
#    CH8DBG        — diagnostic interface, conditional breakpoint matrix
#    EMUGame       — SDL3 window + renderer + game loop
# ────────────────────────────────────────────────────────────────

CXX      := g++
SDL3_DIR := /opt/homebrew/opt/sdl3

# ─── High-Performance Compilation Profile Adjustments ────────────
# If you invoke 'make DEBUG=1', we add debug symbols and toggle flags.
# Otherwise, we default to highly optimized headless production bytecode flags.
ifeq ($(DEBUG),1)
    DEBUG_FLAGS := -g -DCH8_DEBUG_ENABLED=1
    BUILD_SUB   := debug
else
    DEBUG_FLAGS := -O3 -DCH8_DEBUG_ENABLED=0
    BUILD_SUB   := release
endif

CXXFLAGS  := -Wall -Wextra -std=c++17           \
              -I$(SDL3_DIR)/include              \
              -I./Source                          \
              -Wno-unused-parameter              \
              $(DEBUG_FLAGS)

LDFLAGS   := -L$(SDL3_DIR)/lib -lSDL3

SRC_DIR  := ./Source
OBJ_DIR  := ./Build/$(BUILD_SUB)
TARGET   := ./Build/chip8emu_$(BUILD_SUB)

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all run clean lint typecheck help

all: $(TARGET)

$(OBJ_DIR):
	@mkdir -p $@

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@
	@echo "→ Compilation Target Formed Natively: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	$(TARGET) 6-keypad.ch8

clean:
	rm -rf ./Build
	@echo "All workspace binary objects scrubbed cleanly."

lint:
	cppcheck --enable=all --error-exitcode=1 \
             -DCH8_DEBUG_ENABLED=1 \
             -I$(SDL3_DIR)/include -I./Source $(SRCS)

typecheck:
	clang-tidy $(SRCS) -- -std=c++17 -DCH8_DEBUG_ENABLED=1 -I$(SDL3_DIR)/include -I./Source

help:
	@echo "CHIP-8 Emulator Build Subsystem Quick Options Reference Menu:"
	@echo "  make               - Build the default high-performance optimization release binary"
	@echo "  make DEBUG=1       - Build with full interactive debugger capabilities active"
	@echo "  make run           - Compile and run standard testing logo environment"
	@echo "  make run DEBUG=1   - Compile and execute using active debug diagnostic layouts"
	@echo "  make clean         - Reset structural output tracking spaces"