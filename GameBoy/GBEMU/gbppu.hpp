#ifndef GBPPU_HPP
#define GBPPU_HPP

#include <cstdint>
#include <vector>

class GBEMU;

class GBPPU {
public:
    GBPPU(GBEMU* emu);

    void step();
    bool isRefreshRequested() const { return refreshRequested; }
    void clearRefreshRequest() { refreshRequested = false; }
    const std::vector<Uint8>& getFrameBuffer() const { return frameBuffer; }

    int width;
    int height;

private:
    friend class GBEMU;
    
    GBEMU* emu;
    bool refreshRequested = false;
    std::vector<Uint8> frameBuffer;
    std::vector<Uint8> internalFrameBuffer;
    int dot;
    int line;
    Uint8 color;
};

#endif /* GBPPU_HPP */