#include "nesppu.hpp"
#include <SDL3/SDL_stdinc.h>
#include "nesemu.hpp"
#include "nesdsk.hpp"
#include "nescpu.hpp"

// TBD: latch between CPU and PPU bus implementation

NESPPU::NESPPU(NESEMU* emu) : NESComponent(emu) {
    SDL_memset(palette, 0x00, sizeof(palette));
    SDL_memset(oam, 0x00, sizeof(oam));
    SDL_memset(vram, 0x00, sizeof(vram));

    // Initialize frame buffers with black
    for (int i = 0; i < 2; i++) {
        frameBuffers[i] = (Uint8 *)SDL_malloc(sizeof(Uint8) * width * height * 4);
        SDL_memset(frameBuffers[i], 0x00, sizeof(Uint8) * width * height * 4); // Fill with black (0, 0, 0, 0)
    }
}

NESPPU::~NESPPU() {
    for(int i = 0; i < 2; i++) {
        SDL_free(frameBuffers[i]);
    }
}

void NESPPU::powerOn() {
    SDL_Log("NESPPU: powerOn()...");

    // Clears registers completely
    PPUCTRL       = 0x00;     // $2000
    PPUMASK       = 0x00;     // $2001
    PPUSTATUS     = 0x00;     // $2002
    OAMADDR       = 0x00;     // $2003
    OAMDATA       = 0x00;     // $2004
    PPUSCROLL     = 0x00;     // $2005
    PPUADDR       = 0x00;     // $2006
    PPUDATA       = 0x00;     // $2007
    OAMDMA        = 0x00;     // $4014

    // Initialize internal location coordinates
    frameId = 0;
    cycles = 0;
    scanline = 0;
    colour = 0x00;

    // Wipe internal memory domains to ensure no residual data
    SDL_memset(vram, 0x00, sizeof(vram));

    // Initialize frame buffers with black
    for (int i = 0; i < 2; i++) {
        SDL_memset(frameBuffers[i], 0x00, sizeof(Uint8) * width * height * 4); // Fill with white (255, 255, 255, 255)
    }
}

void NESPPU::reset() {
    SDL_Log("NESPPU: reset()...");

    // Clears registers completely
    PPUCTRL       = 0x00;     // $2000
    PPUMASK       = 0x00;     // $2001
    MASK_CLEAR(PPUSTATUS, ~FLAG_VBLANK);       // Clear all other bits
    PPUSCROLL     = 0x00;     // $2005
    PPUDATA       = 0x00;     // $2007

    // Initialize internal location coordinates
    frameId = 0;
    cycles = 0;
    scanline = 0;
    colour = 0x00;

    // Wipe internal memory domains to ensure no residual data
    SDL_memset(vram, 0x00, sizeof(vram));

    // Initialize frame buffers with black
    for (int i = 0; i < 2; i++) {
        SDL_memset(frameBuffers[i], 0x00, sizeof(Uint8) * width * height * 4); // Fill with white (255, 255, 255, 255)
    }
}

const Uint8 *NESPPU::getFrameBuffer() const {
    return frameBuffers[(frameId + 1) % 2];     // Return the non-active buffer for rendering
}

Uint8 NESPPU::read(Uint16 address) {
    // 1. Check if the read request is targeting CPU memory-mapped registers ($2000-$3FFF)
    // The 8 hardware registers are mirrored every 8 bytes across this region.
    Uint16 regOffset = address % 8;

    switch (regOffset) {
        case 2: {               // $2002 PPUSTATUS
            SDL_Log("NESPPU: read PPUSTATUS = %02X", PPUSTATUS);
            Uint8 temp = PPUSTATUS;
            // CRITICAL SIDE EFFECT: Reading PPUSTATUS clears bit 7 (V-Blank status flag)
            // and resets the internal PPU scroll/address latch flip-flop.
            MASK_CLEAR(PPUSTATUS, FLAG_VBLANK);
            return temp;
        }
        case 4: {               // $2004 OAMDATA
            SDL_Log("NESPPU: read OAMDATA = %02X", OAMDATA);
            return OAMDATA;
        }
        case 7: {               // $2007 PPUDATA
            SDL_Log("NESPPU: read PPUDATA = %02X", PPUDATA);
            // Reading from PPUDATA retrieves bytes out of PPU VRAM.
            // TBD: Implement VRAM buffered read sequencing here later.
            return PPUDATA;
        }
    }

    return 0x00;
}

void NESPPU::write(Uint16 address, Uint8 value) {
    Uint16 regOffset = address % 8;

    switch (regOffset) {
        case 0: // $2000 PPUCTRL
            SDL_Log("NESPPU: write PPUCTRL = %02X", value);
            PPUCTRL = value;
            // If the CPU enables NMIs while the PPU is already in a V-Blank period,
            // an NMI is generated immediately.
            if (MASK_CHECK_SET(PPUCTRL, FLAG_NMI_ENABLE) && MASK_CHECK_SET(PPUSTATUS, FLAG_VBLANK)) {
                emu->cpu->nmi_asserted = true;
            }
            return;
        case 1: // $2001 PPUMASK
            SDL_Log("NESPPU: write PPUMASK = %02X", value);
            PPUMASK = value;
            return;
        case 3: // $2003 OAMADDR
            SDL_Log("NESPPU: write OAMADDR = %02X", value);
            OAMADDR = value;
            return;
        case 4: // $2004 OAMDATA
            SDL_Log("NESPPU: write OAMDATA = %02X", value);
            OAMDATA = value;
            return;
        case 5: // $2005 PPUSCROLL
            SDL_Log("NESPPU: write PPUSCROLL = %02X", value);
            // TBD: Feed scroll offsets into internal registers (X/Y latching)
            PPUSCROLL = value;
            break;
        case 6: // $2006 PPUADDR
            SDL_Log("NESPPU: write PPUADDR = %02X", value);
            // TBD: Sequence high byte then low byte to update the current VRAM pointer address
            PPUADDR = value;
            break;
        case 7: // $2007 PPUDATA
            SDL_Log("NESPPU: write PPUDATA = %02X", value);
            // TBD: Write value directly to PPU VRAM via the current internal PPUADDR register pointer, 
            // then automatically increment the register pointer by either 1 or 32 based on PPUCTRL.
            PPUDATA = value;
            break;
        default:
            break;
    }
}

void NESPPU::writeDMA(Uint8 value) {
    SDL_Log("NESPPU: write OAMDMA = %02X", value);
    // TBD: writing on the OAM DMA
    OAMDMA = value;
}

void NESPPU::step() {
    switch (phase) {
        case preRendering:


            break;
        case visibleRendering:
            if (cycles < 256) {
                int index = (scanline * width + cycles) * 4;
                frameBuffers[(frameId % 2)][index + 0] = colour;     // Red
                frameBuffers[(frameId % 2)][index + 1] = colour;     // Green
                frameBuffers[(frameId % 2)][index + 2] = colour;     // Blue
                frameBuffers[(frameId % 2)][index + 3] = 0xFF;      // Alpha
            }
            break;
        case postRendering:
            break;
        case verticalBlank:
            break;
    }

    cycles++;
    if (cycles == 1) {
        if (scanline == 0) {
            SDL_Log("NESPPU: Visible Rendering Phase");
            phase = visibleRendering;
        } else if (scanline == 240) {
            SDL_Log("NESPPU: Post Rendering Phase");
            phase = postRendering;
        } else if (scanline == 241) {
            colour = (colour + 1) % 256;                // DEBUG: uniform colour changing
            MASK_SET(PPUSTATUS, FLAG_VBLANK);
            if MASK_CHECK_SET(PPUCTRL, FLAG_NMI_ENABLE) {
                emu->cpu->nmi_asserted = true;          // Signal NMI to NESCPU
            }
            emu->raiseEvent(NESEvent::VBLANK_START);    // Signal VBlank to NESEMU

            SDL_Log("NESPPU: Vertical Blank Phase");
            phase = verticalBlank;
        } else if (scanline == 261) {
            MASK_CLEAR(PPUSTATUS, FLAG_VBLANK);
            frameId = (frameId + 1) % 2;
            emu->raiseEvent(NESEvent::FRAME_COMPLETE);

            SDL_Log("NESPPU: Pre Rendering Phase");
            phase = preRendering;
        }
    }
    if (cycles >= 342) {
        scanline++;
        cycles = 0;
        if (scanline > 261) {
            scanline = 0;
        }
    }
}