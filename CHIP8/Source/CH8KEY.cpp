#include "CH8KEY.hpp"

CH8KEY::CH8KEY(CH8EMU* parentEmu) : CH8COM(parentEmu) {
    reset();
}

void CH8KEY::powerOn() {
    reset();
}

void CH8KEY::powerOff() {
    reset();
}

void CH8KEY::reset() {
    SDL_memset(keysState, 0, sizeof(keysState));
    isWaitingForKey = false;
    latchedKeyIndex = -1;
}

void CH8KEY::setKeyState(Uint8 keyIndex, Uint8 state) {
    if (keyIndex < 16) {
        keysState[keyIndex] = state;
    }
}

bool CH8KEY::isKeyPressed(Uint8 keyIndex) const {
    if (keyIndex < 16) {
        return keysState[keyIndex] != 0;
    }
    return false;
}

void CH8KEY::lockKeyWait(Uint8 keyIndex) {
    isWaitingForKey = true;
    latchedKeyIndex = static_cast<int>(keyIndex);
}

void CH8KEY::clearKeyWait() {
    isWaitingForKey = false;
    latchedKeyIndex = -1;
}

bool CH8KEY::isCurrentlyWaiting() const {
    return isWaitingForKey;
}

int CH8KEY::getLatchedKey() const {
    return latchedKeyIndex;
}