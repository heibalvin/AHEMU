#include "gbppu.hpp"
#include "gbemu.hpp"
#include <algorithm>

GBPPU::GBPPU(GBEMU* emu)
    : width(160),
      height(144),
      emu(emu),
      refreshRequested(false),
      frameBuffer(width * height * 4, 0),
      internalFrameBuffer(width * height * 4, 0),
      dot(0),
      line(0),
      color(0) {
    (void)this->emu;
}

void GBPPU::step() {
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
            std::swap(frameBuffer, internalFrameBuffer);
            refreshRequested = true;
            line = 0;
        }
    }
}