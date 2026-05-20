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
    void reset() override;
    void step() override;

    Uint8 read(Uint16 address) override;
    void write(Uint16 address, Uint8 value) override;
    void writeDMA(Uint8 value);

    const Uint8 *getFrameBuffer() const;

private:
    friend class NESEMU;
    friend class NESBUS;

    enum RenderingPhase {
        preRendering,
        visibleRendering,
        postRendering,
        verticalBlank
    };

    // double frame buffer
    const int width = 256;
    const int height = 240;
    Uint8 *frameBuffers[2];
    
    // NES PPU variables
    RenderingPhase phase = preRendering;  // Rendering Phase
    int frameId = 0;                            // Current frameId used for frameBuffers too.
    int cycles = 0;                             // Current cycle (dot) within the scanline
    int scanline = 0;                           // Current scanline (0-261)
    Uint8 colour = 0x00;                        // DEBUG: uniform colour background

    // CPU-facing register tracking variables
    Uint8 PPUCTRL       = 0x00;     // $2000
    Uint8 PPUMASK       = 0x00;     // $2001
    Uint8 PPUSTATUS     = 0x00;     // $2002
    Uint8 OAMADDR       = 0x00;     // $2003
    Uint8 OAMDATA       = 0x00;     // $2004
    Uint8 PPUSCROLL     = 0x00;     // $2005
    Uint8 PPUADDR       = 0x00;     // $2006
    Uint8 PPUDATA       = 0x00;     // $2007
    Uint8 OAMDMA        = 0x00;     // $4014

    const Uint8 FLAG_NMI_ENABLE   = 0x80;
    const Uint8 FLAG_VBLANK       = 0x80;

    // Sprite and VRAM variables
    Uint8 palette[8 * 4];           // Palette RAM for background and sprites
    Uint8 oam[64 * 4];              // Object Attribute Memory (OAM) for sprites
    Uint8 vram[4 * 1024];           // VRAM for nametables and attribute tables
};

#endif /* NESPPU_HPP */