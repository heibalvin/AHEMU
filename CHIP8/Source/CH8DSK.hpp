#pragma once
#include <SDL3/SDL.h>
#include "CH8COM.hpp"

class CH8DSK : public CH8COM {
private:
    Uint8* romStorage;
    size_t romStorageSize;

public:
    CH8DSK(CH8EMU* parentEmu);
    ~CH8DSK();

    void powerOn()  override;
    void powerOff() override;
    void reset()    override;

    // Persistently clones incoming data using clean, low-dependency memory blocks
    bool insertRom(const Uint8* datas, size_t size);

    // Hardware alignment accessor lines
    const Uint8* getRomData() const { return romStorage; }
    size_t       getRomSize() const { return romStorageSize; }
    bool         hasRom()     const { return (romStorage != nullptr && romStorageSize > 0); }
};