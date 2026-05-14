CXX = g++
SDL3_DIR = /opt/homebrew/opt/sdl3
LIBPNG_DIR = /opt/homebrew/opt/libpng
CXXFLAGS = -Wall -std=c++17 -I$(SDL3_DIR)/include -I$(LIBPNG_DIR)/include
LDFLAGS = -L$(SDL3_DIR)/lib -lSDL3 -L$(LIBPNG_DIR)/lib -lpng16

SRC_DIR = .
SRC_MAIN = main.cpp gbemu.cpp gbcpu.cpp gbppu.cpp gbdsk.cpp gbbus.cpp gbcomponent.cpp sdlemu.cpp

OBJ_DIR = ../../Build/GBEMU
OBJ_MAIN = $(OBJ_DIR)/main.o $(OBJ_DIR)/gbemu.o $(OBJ_DIR)/gbcpu.o $(OBJ_DIR)/gbppu.o $(OBJ_DIR)/gbdsk.o $(OBJ_DIR)/gbbus.o $(OBJ_DIR)/gbcomponent.o $(OBJ_DIR)/sdlemu.o

TARGET_MAIN = $(OBJ_DIR)/gbemu

all: main

main: $(OBJ_DIR) $(TARGET_MAIN)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET_MAIN):  $(OBJ_MAIN)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: main
	./$(TARGET_MAIN)

clean:
	rm -f $(OBJ_DIR)/*

.PHONY: all main run clean