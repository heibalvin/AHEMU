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
    clearKeys();
}

void CH8CON::clearKeys() {
    SDL_memset(KEYS, 0, sizeof(KEYS));
}

Uint8 CH8CON::isKeyPressed(Uint8 keyIndex) const {
    return (keyIndex < 16) ? KEYS[keyIndex] : 0;
}

void CH8CON::setKeyState(Uint8 keyIndex, Uint8 isPressed) {
    if (keyIndex < 16) {
        KEYS[keyIndex] = (isPressed ? 1 : 0);
    }
}