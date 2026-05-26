#include "CH8CON.hpp"

CH8CON::CH8CON(CH8EMU* parentEmu) : CH8COM(parentEmu) {
    reset();
}

void CH8CON::powerOn() {
    reset();
}

void CH8CON::powerOff() {
    reset();
}

void CH8CON::reset() {
    SDL_memset(keysState, 0, sizeof(keysState));
    isWaitingForKey = false;
    latchedKeyIndex = -1;
}

void CH8CON::setKeyState(Uint8 keyIndex, Uint8 state) {
    if (keyIndex < 16) {
        keysState[keyIndex] = state;
    }
}

bool CH8CON::isKeyPressed(Uint8 keyIndex) const {
    if (keyIndex < 16) {
        return keysState[keyIndex] != 0;
    }
    return false;
}

void CH8CON::lockKeyWait(Uint8 keyIndex) {
    isWaitingForKey = true;
    latchedKeyIndex = static_cast<int>(keyIndex);
}

void CH8CON::clearKeyWait() {
    isWaitingForKey = false;
    latchedKeyIndex = -1;
}

bool CH8CON::isCurrentlyWaiting() const {
    return isWaitingForKey;
}

int CH8CON::getLatchedKey() const {
    return latchedKeyIndex;
}