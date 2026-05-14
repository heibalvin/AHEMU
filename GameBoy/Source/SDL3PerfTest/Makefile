CXX = g++
SDL3_DIR = /opt/homebrew/opt/sdl3
CXXFLAGS = -Wall -std=c++17 -I$(SDL3_DIR)/include
LDFLAGS = -L$(SDL3_DIR)/lib -lSDL3

SRC_DIR = .
SRC_MAIN = main.cpp

OBJ_DIR = ../../Build/SDL3PerfTest
OBJ_MAIN = $(OBJ_DIR)/main.o

TARGET_MAIN = $(OBJ_DIR)/sdl3perftest

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