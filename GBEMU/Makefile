CXX = g++
SDL3_DIR = /opt/homebrew/opt/sdl3
CXXFLAGS = -Wall -std=c++17 -I$(SDL3_DIR)/include -I./Source/Include
LDFLAGS = -L$(SDL3_DIR)/lib -lSDL3

SRC_DIR = ./Source
SRC_MAIN = $(SRC_DIR)/main.cpp $(SRC_DIR)/gbemu.cpp $(SRC_DIR)/gbcpu.cpp $(SRC_DIR)/gbppu.cpp $(SRC_DIR)/gbdsk.cpp $(SRC_DIR)/gbbus.cpp $(SRC_DIR)/gbcomponent.cpp $(SRC_DIR)/sdlemu.cpp

OBJ_DIR = ./Build
OBJ_MAIN = $(OBJ_DIR)/main.o $(OBJ_DIR)/gbemu.o $(OBJ_DIR)/gbcpu.o $(OBJ_DIR)/gbppu.o $(OBJ_DIR)/gbdsk.o $(OBJ_DIR)/gbbus.o $(OBJ_DIR)/gbcomponent.o $(OBJ_DIR)/sdlemu.o

TARGET_MAIN = $(OBJ_DIR)/gbemu

all: main

main: $(OBJ_DIR) $(TARGET_MAIN)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET_MAIN): $(OBJ_MAIN)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: main
	./$(TARGET_MAIN)

clean:
	rm -f $(OBJ_DIR)/*

.PHONY: all main run clean