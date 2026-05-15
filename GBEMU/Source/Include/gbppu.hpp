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
    
    // GB Window varaibles
    const int width = 160;
    const int height = 144;
    Uint8 **frameBuffers;
    int activeFrameBuffer = 0;
    bool isRefreshRequested = false;
    
    // GB PPU varaibles
    int dot = 0;
    int line = 0;
    Uint8 color = 0;
};

#endif /* GBPPU_HPP */