#ifndef NESEMU_HPP
#define NESEMU_HPP

#include <cstdint>
#include "nesppu.hpp"

 class NESEMU {
 public:
     NESEMU();
     void update(double currentTime);
     void step();
     bool isRefreshRequested() const;
     void clearRefreshRequest();
     const std::vector<uint8_t>& getFrameBuffer() const;

     double masterClock;
     double clock;

 private:
     double previousTime = 0.0;
     double deltaTime = 0.0;
     uint64_t cycleId = 0;
     NESPPU ppu;
 };

#endif /* NESEMU_HPP */