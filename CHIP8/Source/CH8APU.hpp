#pragma once
#include "CH8COM.hpp"

class CH8APU : public CH8COM {
private:
    bool buzzerActive = false;

public:
    CH8APU(CH8EMU* parentEmu);
    ~CH8APU() = default;

    void powerOn() override;
    void powerOff() override;
    void reset() override;
    void step() override;
    void stop();
    bool isBuzzerActive() const;
};