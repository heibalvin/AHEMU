#include "nesppu.hpp"
#include "nesemu.hpp"
#include <algorithm>

NESPPU::NESPPU(NESEMU& emu)
    : width(256),
      height(240),
      emu(emu),
      frameBuffer(width * height * 4, 0),
      internalFrameBuffer(width * height * 4, 0),
      isRefreshReq(false),
      dot(0),
      line(0),
      color(0) {
    (void)this->emu;
}

void NESPPU::step() {
    int index = (line * width + dot) * 4;

    internalFrameBuffer[index + 0] = color;
    internalFrameBuffer[index + 1] = color;
    internalFrameBuffer[index + 2] = color;
    internalFrameBuffer[index + 3] = 0xFF;

    color = (color + 1) % 256;

    dot++;
    if (dot >= width) {
        dot = 0;
        line++;
        if (line >= height) {
            // Frame complete: swap buffers and signal refresh
            std::swap(frameBuffer, internalFrameBuffer);
            isRefreshReq = true;
            line = 0;
        }
    }
}