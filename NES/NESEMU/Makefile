CXX = g++
SDL3_DIR = /opt/homebrew/opt/sdl3
CXXFLAGS = -Wall -std=c++17 -I$(SDL3_DIR)/include -I./Source
LDFLAGS = -L$(SDL3_DIR)/lib -lSDL3

SRC_MAIN = Source/main.cpp Source/nesemu.cpp Source/nesppu.cpp

OBJ_DIR = Build
OBJ_MAIN = $(OBJ_DIR)/main.o $(OBJ_DIR)/nesemu.o $(OBJ_DIR)/nesppu.o

TARGET_MAIN = $(OBJ_DIR)/nesemu

all: main

main: $(TARGET_MAIN)

$(TARGET_MAIN): $(OBJ_MAIN)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: Source/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET_CALLBACKS): $(OBJ_CALLBACKS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_CALLBACKS): $(SRC_CALLBACKS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: main
	./$(TARGET_MAIN)

clean:
	rm -f $(OBJ_DIR)/*

.PHONY: all main run clean