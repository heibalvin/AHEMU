#include "GBVDP.h"
#include "GBEMU.h"

GBVDP::GBVDP(GBEMU* emu) : GBCOM(emu) {
    backBufferIdx = 0;
    reset();
}

void GBVDP::reset() {
// Use SDL_memset for high-performance zeroing of memory
    SDL_memset(VRAM, 0, sizeof(VRAM));
    SDL_memset(OAM, 0, sizeof(OAM));
    SDL_memset(pixelFIFO, 0, sizeof(pixelFIFO));
    
    // Initialize registers to zero
    LCDC = 0;
    STAT = 0;
    LY   = 0;
    SCX  = 0;
    SCY  = 0;
    BGP  = 0;
    WX   = 0;
    WY   = 0;
    LYC  = 0;
    
    // Reset internal state machines
    fifoReadIdx  = 0;
    fifoWriteIdx = 0;
    fifoCount    = 0;
    fetcherState = 0;
    
    // Reset DMA state
    dmaActive    = false;
    dmaSource    = 0;
    dmaCycles    = 0;
    
    // Reset cycle counter
    windowLine = 0; // Reset at the start of the system
    cycle = 0;
}

Uint8 GBVDP::read(Uint16 addr) {
    // 1. VRAM (0x8000 - 0x9FFF)
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        return VRAM[addr - 0x8000];
    }
    // 2. OAM (0xFE00 - 0xFE9F)
    if (addr >= 0xFE00 && addr <= 0xFE9F) {
        return OAM[addr - 0xFE00];
    }
    // 3. Registers (0xFF40 - 0xFF4B)
    switch (addr) {
        case 0xFF40: return LCDC;
        case 0xFF41: return STAT;
        case 0xFF42: return SCY;
        case 0xFF43: return SCX;
        case 0xFF44: return LY;
        case 0xFF45: return LYC;
        case 0xFF47: return BGP;
        case 0xFF4A: return WY;
        case 0xFF4B: return WX;
        default: return 0xFF; // Open bus
    }
}

void GBVDP::write(Uint16 addr, Uint8 value) {
    // 1. VRAM (0x8000 - 0x9FFF)
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        VRAM[addr - 0x8000] = value;
        return;
    }
    // 2. OAM (0xFE00 - 0xFE9F)
    if (addr >= 0xFE00 && addr <= 0xFE9F) {
        OAM[addr - 0xFE00] = value;
        return;
    }
    // 3. Registers
    switch (addr) {
        case 0xFF40: LCDC = value; break;
        case 0xFF41: STAT = (STAT & 0x07) | (value & 0xF8); break; // Bits 0-2 are Read-Only
        case 0xFF42: SCY = value; break;
        case 0xFF43: SCX = value; break;
        case 0xFF45: LYC = value; break;
        case 0xFF46: /* OAM DMA Trigger - handled in update() logic */ 
                     dmaSource = value; dmaActive = true; dmaCycles = 0; break;
        case 0xFF47: BGP = value; break;
        case 0xFF4A: WY = value; break;
        case 0xFF4B: WX = value; break;
    }
}

void GBVDP::swapBuffers() {
    frontBuffer = buffers[backBufferIdx];
    backBufferIdx = 1 - backBufferIdx;
}

void GBVDP::update(Uint8 cycles) {
    // 1. OAM DMA Logic (High-priority cycle tracking)
    if (dmaActive) {
        dmaCycles += (cycles * 4);
        if (dmaCycles >= 160) {
            for (int i = 0; i < 160; ++i) {
                OAM[i] = emu->bus.read((dmaSource << 8) + i);
            }
            dmaActive = false;
        }
    }

    // 2. LCD Power Check
    if (!(LCDC & 0x80)) {
        cycle = 0;
        LY = 0;
        STAT &= 0xFC; // Reset to Mode 0
        return;
    }

    cycle += (cycles * 4);
    Uint8 mode = STAT & 0x03;

    // 3. Mode State Machine
    switch (mode) {
        case 2: // OAM Search
            if (cycle >= 80) {
                cycle -= 80;
                oamSearch();
                STAT = (STAT & 0xFC) | 3; // Switch to Pixel Transfer
                LX = 0;
                fetcherState = 0; // Reset FIFO fetcher
            }
            break;

        case 3: // Pixel Transfer
            // Fetcher advances independent of pixel output
            tickFetcher();
            
            // Consume from FIFO to output to screen
            if (cycle >= 1 && fifoCount > 0) {
                Pixel p = popPixel();
                buffers[backBufferIdx][LY * 160 + LX] = PALETTE[p.color];
                LX++;
                cycle -= 1;
            }
            
            if (LX >= 160) {
                STAT = (STAT & 0xFC) | 0; // H-Blank
                checkSTATInterrupts();
            }
            break;

        case 0: // H-Blank
            if (cycle >= 204) {
                cycle -= 204;

                // Check if the current line was inside the Window area
                if (LY >= WY && (LCDC & 0x20)) {
                    windowLine++;
                }

                LY++;
                if (LY == LYC) { STAT |= 0x04; checkSTATInterrupts(); } 
                else { STAT &= ~0x04; }

                if (LY == 144) {
                    windowLine = 0;
                    STAT = (STAT & 0xFC) | 1; // V-Blank
                    // Trigger V-Blank Interrupt
                    emu->bus.write(0xFF0F, emu->bus.read(0xFF0F) | 0x01);
                    checkSTATInterrupts();
                    swapBuffers();
                } else {
                    STAT = (STAT & 0xFC) | 2; // OAM Search
                    checkSTATInterrupts();
                }
            }
            break;

        case 1: // V-Blank
            if (cycle >= 456) {
                cycle -= 456;
                LY++;
                if (LY > 153) {
                    LY = 0;
                    STAT = (STAT & 0xFC) | 2; // Restart frame
                    checkSTATInterrupts();
                }
            }
            break;
    }
}

void GBVDP::renderPixel() {
    if (LX >= 160) return; // Ensure we don't exceed scanline width

    // 1. Fetch Background or Window color index (0-3)
    Uint8 bgIdx = getBackgroundOrWindowPixel();
    
    // 2. Map color index to the BGP palette register
    // BGP is a 8-bit register where each 2 bits define a color: 
    // e.g., bits 7-6 define color 3, 5-4 define color 2, etc.
    Uint32 finalColor = PALETTE[(BGP >> (bgIdx * 2)) & 0x03];

    // 3. Process Sprite mixing
    // If LCDC bit 1 is set, sprites are enabled.
    if (LCDC & 0x02) {
        processSpriteMixing(bgIdx, finalColor);
    }

    // 4. Output to active buffer
    // BackBufferIdx toggles between 0 and 1 to prevent tearing
    buffers[backBufferIdx][LY * 160 + LX] = finalColor;
    
    // Increment X position for the next pixel
    LX++;
}

Uint8 GBVDP::getBackgroundOrWindowPixel() {
    // 1. Determine if we are rendering the Window or the Background
    // Window is enabled if LCDC bit 5 is set, LY >= WY, and LX >= WX - 7
    bool useWindow = (LCDC & 0x20) && (LY >= WY) && (LX >= (WX - 7));
    
    // 2. Calculate coordinate relative to the map
    // mapY: Vertical coordinate in the map
    // mapX: Horizontal coordinate in the map
    Uint8 mapY = useWindow ? windowLine : ((LY + SCY) & 0xFF);
    Uint8 mapX = useWindow ? (LX - (WX - 7)) : ((LX + SCX) & 0xFF);
    
    // 3. Find the Tile ID in the background/window tile map
    // Tile map base address is determined by LCDC bits (0x1800 or 0x1C00 relative to 0x8000)
    Uint16 mapBase = (useWindow ? (LCDC & 0x40) : (LCDC & 0x08)) ? 0x1C00 : 0x1800;
    Uint8 tileId = emu->bus.read(0x8000 + mapBase + (mapY / 8) * 32 + (mapX / 8));

    // 4. Determine tile data location
    // Tile data base is determined by LCDC bit 4
    // If bit 4 is 0, tile IDs are signed (-128 to 127)
    Uint16 tileBase = (LCDC & 0x10) ? 0x0000 : 0x0800;
    Uint16 tileAddr = (LCDC & 0x10) ? (tileId * 16) : ((int8_t)tileId + 128) * 16;
    Uint16 pixelRowAddr = 0x8000 + tileBase + tileAddr + ((mapY % 8) * 2);

    // 5. Fetch the 2 bits of color data for this pixel
    Uint8 byte1 = emu->bus.read(pixelRowAddr);
    Uint8 byte2 = emu->bus.read(pixelRowAddr + 1);
    
    // Bit position in the byte (7 to 0)
    int bitPos = 7 - (mapX % 8);
    
    // Combine bits to form 2-bit color index (0-3)
    return ((byte2 >> bitPos) & 1) << 1 | ((byte1 >> bitPos) & 1);
}

Uint8 GBVDP::fetchSpritePixel(Uint8 idx, Uint8 &sFlags) {
    // 1. Calculate the OAM address for the sprite (4 bytes per sprite)
    Uint16 addr = 0xFE00 + (idx * 4);
    
    // 2. Read sprite attributes from the Bus
    Uint8 sY = emu->bus.read(addr);
    Uint8 sX = emu->bus.read(addr + 1);
    Uint8 sTile = emu->bus.read(addr + 2);
    sFlags = emu->bus.read(addr + 3); // Store flags for palette/priority logic

    // 3. Check if the current LX coordinate overlaps with the sprite
    // Sprites are 8 pixels wide, and sX/sY are offset by 8 and 16 respectively
    if (LX >= (sX - 8) && LX < (sX - 8 + 8)) {
        int height = (LCDC & 0x04) ? 16 : 8;
        int row = LY + 16 - sY;
        
        // 4. Handle Vertical Flip
        if (sFlags & 0x40) row = (height - 1) - row; 

        // 5. Fetch tile data from VRAM
        // Each tile is 16 bytes (8 lines * 2 bytes/line)
        Uint16 sTileAddr = 0x8000 + (sTile * 16) + (row * 2);
        Uint8 sByte1 = emu->bus.read(sTileAddr);
        Uint8 sByte2 = emu->bus.read(sTileAddr + 1);

        // 6. Handle Horizontal Flip
        int col = LX - (sX - 8);
        int sBitPos = (sFlags & 0x20) ? col : (7 - col); 
        
        // 7. Return 2-bit color index
        return ((sByte2 >> sBitPos) & 1) << 1 | ((sByte1 >> sBitPos) & 1);
    }
    
    // Return 0 if no overlap or no pixel data
    return 0; 
}

void GBVDP::processSpriteMixing(Uint8 bgIdx, Uint32 &finalColor) {
    // Iterate through the sprites found during OAM Search (max 10 per line)
    for (int i = 0; i < spriteCount; ++i) {
        Uint8 sFlags = 0;
        
        // Retrieve the 2-bit color index for this specific pixel from the sprite tile
        Uint8 sColorIdx = fetchSpritePixel(spriteIndices[i], sFlags);

        // Color 0 is always transparent for sprites
        if (sColorIdx != 0) { 
            // Priority check (Bit 7 of sFlags): 
            // If set, sprite is behind background, 
            // UNLESS the background color index is 0 (transparent).
            bool behindBg = (sFlags & 0x80) && (bgIdx != 0);

            if (!behindBg) {
                // Select the correct palette (OBP0 or OBP1) based on flag bit 4
                Uint8 palette = (sFlags & 0x10) ? OBP1 : OBP0;
                
                // Map the 2-bit index to the palette color
                finalColor = PALETTE[(palette >> (sColorIdx * 2)) & 0x03];
                
                // Return immediately because we found the highest priority sprite 
                // for this pixel (OAM order implies priority)
                return; 
            }
        }
    }
}

void GBVDP::oamSearch() {
    spriteCount = 0; // Reset counter for the current line
    
    // 1. Determine sprite height based on LCDC bit 2
    // 0 = 8x8, 1 = 8x16
    int height = (LCDC & 0x04) ? 16 : 8;

    // 2. Iterate through OAM (40 entries total, 4 bytes each)
    for (int i = 0; i < 40 && spriteCount < 10; ++i) {
        Uint16 addr = 0xFE00 + (i * 4);
        Uint8 sY = emu->bus.read(addr);
        
        // 3. Check if sprite intersects the current LY
        // The hardware offset for Y is 16.
        // A sprite is visible if (LY + 16) is within [sY, sY + height)
        if (LY + 16 >= sY && LY + 16 < sY + height) {
            spriteIndices[spriteCount] = i;
            spriteCount++;
        }
    }
}

void GBVDP::checkSTATInterrupts() {
    // 1. Determine if any condition is currently met
    // Bit 3: Mode 0 (H-Blank)
    // Bit 4: Mode 1 (V-Blank)
    // Bit 5: Mode 2 (OAM Search)
    // Bit 6: LY == LYC match
    
    bool mode0 = (STAT & 0x08) && ((STAT & 0x03) == 0);
    bool mode1 = (STAT & 0x10) && ((STAT & 0x03) == 1);
    bool mode2 = (STAT & 0x20) && ((STAT & 0x03) == 2);
    bool lycMatch = (STAT & 0x40) && (STAT & 0x04); // STAT bit 2 is LY=LYC flag

    // 2. Logic: If any of these are true, trigger the interrupt
    bool interruptTriggered = mode0 || mode1 || mode2 || lycMatch;

    // 3. Trigger the interrupt in the Interrupt Master (0xFF0F)
    if (interruptTriggered) {
        Uint8 ifReg = emu->bus.read(0xFF0F);
        emu->bus.write(0xFF0F, ifReg | 0x02); // Bit 1 of IF is STAT interrupt
    }
}

// Helper functions for the FIFO
void GBVDP::pushPixel(Pixel p) {
    if (fifoCount < FIFO_SIZE) {
        pixelFIFO[fifoWriteIdx] = p;
        fifoWriteIdx = (fifoWriteIdx + 1) % FIFO_SIZE;
        fifoCount++;
    }
}

Pixel GBVDP::popPixel() {
    Pixel p = pixelFIFO[fifoReadIdx];
    fifoReadIdx = (fifoReadIdx + 1) % FIFO_SIZE;
    fifoCount--;
    return p;
}

void GBVDP::tickFetcher() {
    // Each fetcher step takes 2 cycles (or more depending on bus latency)
    switch (fetcherState) {
        case FETCH_TILE_ID:
            // Fetch logic
            fetcherState = FETCH_TILE_DATA0;
            break;
        case FETCH_TILE_DATA0:
            // Fetch logic
            fetcherState = FETCH_TILE_DATA1;
            break;
        case FETCH_TILE_DATA1:
            // Fetch logic
            fetcherState = PUSH_TO_FIFO;
            break;
        case PUSH_TO_FIFO:
            if (fifoCount <= 8) {
                // Push logic
                fetcherState = FETCH_TILE_ID;
            }
            break;
    }
}