#ifndef GBPPU_HPP
#define GBPPU_HPP

#include <SDL3/SDL.h>
#include <vector>
#include "gbcomponent.hpp"

class GBEMU;

class GBPPU : public GBComponent {
public:
    explicit GBPPU(GBEMU* emu);
    ~GBPPU();

    void step() override;
    const Uint8 *getFrameBuffer();

private:
    friend class GBEMU;
    friend class GBBUS;

    // GB Window variables
    const int width = 160;
    const int height = 144;
    Uint8 *frameBuffers[2] = { NULL, NULL};
    int frameBufferActive = 0;
    bool isRefreshRequested = false;
    
    // GB PPU variables
    int dot = 0;
    int line = 0;
    Uint8 color = 0;

    // Sprite and VRAM variables
    Uint8* oam = NULL; // Object Attribute Memory (OAM) for sprites
    Uint8 *vrams[2] = { NULL, NULL };
    int vramCount = 1;
    int vramActive = 0;
};

#endif /* GBPPU_HPP */