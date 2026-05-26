#ifndef CH8CON_HPP
#define CH8CON_HPP

#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8EMU; // Forward declaration of parent container

class CH8CON : public CH8COM {
public:
    CH8CON(CH8EMU* parentEmu);
    virtual ~CH8CON() = default;

    // Implementation of CH8COM abstract hardware interface overrides
    virtual void powerOn() override;
    virtual void powerOff() override;
    virtual void reset() override;

    // Standard keypad hardware operations
    void setKeyState(Uint8 keyIndex, Uint8 state);
    bool isKeyPressed(Uint8 keyIndex) const;

    // FX0A Key release tracking subsystem latch
    void lockKeyWait(Uint8 keyIndex);
    void clearKeyWait();
    bool isCurrentlyWaiting() const;
    int getLatchedKey() const;

private:
    Uint8 keysState[16];      // 1 = Pressed, 0 = Released
    bool isWaitingForKey;     // Active flag during FX0A hardware hold
    int latchedKeyIndex;      // Index of locked active scanner key loop
};

#endif // CH8CON_HPP