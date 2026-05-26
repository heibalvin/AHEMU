CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 `pkg-config --cflags sdl3`
LIBS = `pkg-config --libs sdl3`

# Output directory for all build artifacts
BUILD_DIR = Build
TARGET = $(BUILD_DIR)/CH8EMU
ROM = 1-chip8-logo.ch8

# Discover all .cpp files in the Source directory
SRCS = $(wildcard Source/*.cpp)

# Map source files from Source/*.cpp to target object files in Build/*.o
OBJS = $(patsubst Source/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

# Link the executable from object files inside the Build directory
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile source files directly into the Build directory
# The 'mkdir' command ensures the Build directory exists before compiling
$(BUILD_DIR)/%.o: Source/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run target to build and launch the emulator from its home inside Build/
run: $(TARGET)
	./$(TARGET) $(ROM)

# Clean target removes all built binaries and objects, preserving the folder and .gitkeep
clean:
	@if [ -d $(BUILD_DIR) ]; then \
		find $(BUILD_DIR) -type f ! -name '.gitkeep' -delete; \
	fi

.PHONY: all clean run