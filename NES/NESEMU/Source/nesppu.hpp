#ifndef NESPPU_HPP
#define NESPPU_HPP

#include <cstdint>
#include <vector>

class NESEMU;

class NESPPU {
public:
    NESPPU(NESEMU& emu);

    void step();

    const std::vector<uint8_t>& getFrameBuffer() const { return frameBuffer; }

    int width;
    int height;

 private:
     NESEMU& emu;
     std::vector<uint8_t> frameBuffer;
     std::vector<uint8_t> internalFrameBuffer;
     bool isRefreshReq;
     int dot;
     int line;
     uint8_t color;
 public:
     bool isRefreshRequested() const { return isRefreshReq; }
     void clearRefreshRequest() { isRefreshReq = false; }
};

#endif /* NESPPU_HPP */