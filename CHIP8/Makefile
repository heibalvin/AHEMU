# Compiler Configuration
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra `pkg-config --cflags sdl3`
LDFLAGS  := `pkg-config --libs sdl3`

# Strict Structural Directory Definitions
SRC_DIR  := Source
OBJ_DIR  := Build

# Target Output Binary Placement
TARGET_RELEASE := $(OBJ_DIR)/ch8emu
TARGET_DEBUG   := $(OBJ_DIR)/ch8emu_debug

# Doxygen configuration
DOXYFILE := Doxyfile

# 1. Dynamically find all .cpp source files inside the Source/ folder
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)

# 2. Extract filenames, strip paths, and map them to their Build/ matrix locations
OBJS     := $(patsubst %.cpp, %.o, $(notdir $(SRCS)))

# ---------------------------------------------------------------------
# Master Targets Matrix Configuration
# ---------------------------------------------------------------------

# Running "make" with no arguments now automatically builds BOTH targets
all: release debug

# Running "make release" builds ONLY the optimized production binary
release: $(TARGET_RELEASE)

# Running "make debug" builds ONLY the diagnostic binary with loop logging active
debug: $(TARGET_DEBUG)

# Running "make doxygen" generates the documentation
doxygen: $(DOXYFILE)
	doxygen $(DOXYFILE)

# ---------------------------------------------------------------------
# Binary Generation Rules
# ---------------------------------------------------------------------

$(TARGET_DEBUG): CXXFLAGS += -g -O0 -DDEBUG_MODE
$(TARGET_DEBUG): $(addprefix $(OBJ_DIR)/debug/, $(OBJS))
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Debug target build successfully completed -> $@"

$(TARGET_RELEASE): CXXFLAGS += -O3
$(TARGET_RELEASE): $(addprefix $(OBJ_DIR)/release/, $(OBJS))
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Release target build successfully completed -> $@"

# ---------------------------------------------------------------------
# Compilation Invariant Pattern Matching Rules
# ---------------------------------------------------------------------

# Directs GNU Make to explicitly look inside the Source/ folder for all requirements
vpath %.cpp $(SRC_DIR)

# Compilation rule for Debug object dependencies
$(OBJ_DIR)/debug/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -g -O0 -DDEBUG_MODE -c $< -o $@

# Compilation rule for Release object dependencies
$(OBJ_DIR)/release/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -O3 -c $< -o $@

# ---------------------------------------------------------------------
# Selective Cleanup Configuration
# ---------------------------------------------------------------------
clean:
	rm -rf $(OBJ_DIR)/debug $(OBJ_DIR)/release
	rm -f $(TARGET_RELEASE) $(TARGET_DEBUG)
	@echo "Objects and binaries cleared. Build/ directory and .gitkeep preserved."

.PHONY: all debug release clean doxygen