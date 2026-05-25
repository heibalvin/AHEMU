#pragma once
#include "CH8COM.hpp"

class CH8APU : public CH8COM {
private:
    bool buzzerActive = false;

public:
    CH8APU(CH8EMU* parentEmu) : CH8COM(parentEmu) {}

    void powerOn() override {
        buzzerActive = false;
    }

    void powerOff() override {
        buzzerActive = false;
    }

    void reset() override {
        buzzerActive = false;
    }

    void step() override {
        buzzerActive = true;
    }

    void stop() {
        buzzerActive = false;
    }

    bool isBuzzerActive() const {
        return buzzerActive;
    }
};