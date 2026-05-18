#ifndef NESPPU_HPP
#define NESPPU_HPP

#include <SDL3/SDL.h>
#include "nescomponent.hpp"

class NESEMU;
class NESCPU;

class NESPPU : public NESComponent {
public:
    explicit NESPPU(NESEMU* emu);
    ~NESPPU();

    void powerOn() override;
    void step() override;

    Uint8 read(Uint16 address);
    void write(Uint16 address, Uint8 vlue);

    const Uint8 *getFrameBuffer() const;

private:
    friend class NESEMU;
    friend class NESBUS;

    // SDLEMU variables
    const int width = 256;
    const int height = 240;
    Uint8 *frameBuffers[2] = { NULL, NULL};
    int frameBufferActive = 0;
    bool isRefreshRequested = false;
    
    // NES PPU variables
    int cycles = 0;                     // Current cycle (dot) within the scanline
    int scanline = 0;                   // Current scanline (0-261)

    // NES PPU debug image
    int dot = 0;
    int line = 0;
    Uint8 color = 0;

    // CPU-facing register tracking variables
    Uint8 ppu_ctrl = 0x00;    // $2000
    Uint8 ppu_mask = 0x00;    // $2001
    Uint8 ppu_status = 0x00;  // $2002

    // Sprite and VRAM variables
    Uint8* palRam = NULL;               // Palette RAM for background and sprites
    Uint8* oamRam = NULL;               // Object Attribute Memory (OAM) for sprites
    Uint8* vram = NULL;                 // VRAM for nametables and attribute tables
    Uint8* registers = NULL;            // PPU I/O registers (0x2000-0x2007)
};

#endif /* NESPPU_HPP */