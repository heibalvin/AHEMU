#ifndef GBPPU_HPP
#define GBPPU_HPP

#include <SDL3/SDL.h>
#include <vector>
#include "gbcomponent.hpp"

class GBEMU;

class GBPPU : public GBComponent {
public:
    explicit GBPPU(GBEMU* emu);

    void step(Uint64 deltaTime) override;
    bool isRefreshRequested() const { return refreshRequested; }
    void clearRefreshRequest() { refreshRequested = false; }
    const std::vector<Uint8>& getFrameBuffer() const { return frameBuffer; }

    int width;
    int height;

private:
    friend class GBEMU;
     
    bool refreshRequested = false;
    std::vector<Uint8> frameBuffer;
    std::vector<Uint8> internalFrameBuffer;
    int dot;
    int line;
    Uint8 color;
};

#endif /* GBPPU_HPP */