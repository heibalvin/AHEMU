#include "nesdsk.hpp"

NESDSK::NESDSK(NESEMU* emu) : NESComponent(emu) {}

NESDSK::~NESDSK() {
    if (trnRom) {
        SDL_free(trnRom);
        trnRom = NULL;
    }
    if (prgRoms) {
        for(int i = 0; i < prgRomCount; i++) {
            SDL_free(prgRoms[i]);
            prgRoms[i] = NULL;
        }
        SDL_free(prgRoms);
        prgRoms = NULL;
    }
    if (chrRoms) {
        for(int i = 0; i < chrRomCount; i++) {
            SDL_free(chrRoms[i]);
            chrRoms[i] = NULL;
        }
        SDL_free(chrRoms);
        chrRoms = NULL;
    }
    if (prgRams) {
        for(int i = 0; i < prgRamCount; i++) {
            SDL_free(prgRams[i]);
            prgRams[i] = NULL;
        }
        SDL_free(prgRams);
        prgRams = NULL;
    }
}

void NESDSK::debug() {
    SDL_Log("NESDSK: Mapper: %d", mapper);
    SDL_Log("\tVersion: %d", version);
    SDL_Log("\tRegion: %s", region == 0 ? "NTSC" : "PAL");
    SDL_Log("\tMirroring: %s", mirroring == 0 ? "Horizontal" : (mirroring == 1 ? "Vertical" : "Four-screen"));
    if (trnRomCount != 0) {
        SDL_Log("\tTRN ROM: %d * %d B", trnRomCount, trnRomSize);
    }
    SDL_Log("\tPRG ROM: %d * %d KB", prgRomCount, prgRomSizeKB);
    SDL_Log("\tCHR ROM: %d * %d KB", chrRomCount, chrRomSizeKB);
    if (prgRamCount != 0) {
        SDL_Log("\tPRG RAM: %d * %d KB", prgRamCount, prgRamSizeKB);
    }
}

Uint8 NESDSK::read(Uint16 address) {
    if (address >= 0x6000 && address < 0x8000) {
        // Cartridge space Save/Work RAM (PRG RAM)
        if (prgRams && prgRamCount > 0) {
            return prgRams[0][address - 0x6000];
        }
        return 0x00;
    } 
    
    if (address >= 0x8000 && address < 0xC000) {
        // PRG ROM Lower Bank
        if (prgRoms && prgRomActive[0] < prgRomCount) {
            return prgRoms[prgRomActive[0]][address - 0x8000];
        }
        return 0x00;
    } 
    
    if (address >= 0xC000) { // Goes up to 0xFFFF
        // PRG ROM Upper Bank
        if (prgRoms && prgRomActive[1] < prgRomCount) {
            return prgRoms[prgRomActive[1]][address - 0xC000];
        }
        return 0x00;
    }

    return 0x00; // Fallback for safely ignoring out-of-bounds cartridge queries
}

void NESDSK::write(Uint16 address, Uint8 value) {
    if (address >= 0x6000 && address < 0x8000) {
        // Cartridge space Save/Work RAM (PRG RAM) - Writable
        if (prgRams && prgRamCount > 0) {
            prgRams[0][address - 0x6000] = value;
        }
    } 
    else if (address >= 0x8000) {
        // CPU writing to PRG ROM area ($8000-$FFFF). 
        // Physical ROM chips cannot be overwritten, so these writes are captured 
        // entirely by memory mappers inside the cartridge to switch game banks!
        // TBD: Route 'value' to custom Mapper registers here later.
    }
}

void NESDSK::loadRom(Uint8* gamerom) {
    // Implementation for parsing ROM data and extracting information such as title, cartridge type, ROM size, RAM size, etc.
    if (gamerom[0x00] != 'N' || gamerom[0x01] != 'E' || gamerom[0x02] != 'S' || gamerom[0x03] != 0x1A) {
        SDL_Log("NESDSK: Invalid ROM header");
        return;
    }

    // Extract mapper number (1 byte at 0x0F)
    mapper = (gamerom[0x07] & 0xF0) | (gamerom[0x06] >> 4);

    // Extract compatibility flags
    version = ((gamerom[0x07] & 0x0C) == 0x08) ? 2 : 1; // NES 2.0 if bits 3-2 of byte 7 are 10

    // Extract TV region standard
    region = gamerom[0x08] & 0x01; // 0 for NTSC, 1 for PAL

    // Extract Mirroring type
    mirroring = ((gamerom[0x06] & 0x08) >> 3) | (gamerom[0x06] & 0x01); // 0 for horizontal, 1 for vertical, 2 for four-screen

    // Start of ROM data after header
    Uint16 addr = 0x10;

    // Extract TRN RAM information (if present)
    trnRomSize = 512;                           // TRN ROM size in Bytes
    trnRomCount = (gamerom[0x06] & 0x04) ? 1 : 0;       // TRN ROM present flag
    if (trnRomCount != 0) {
        trnRom = (Uint8 *)SDL_malloc(sizeof(Uint8) * trnRomSize);
        SDL_memcpy(trnRom, &gamerom[addr], trnRomSize);
        addr += trnRomSize;
    }

    // Extract PRG ROM information
    prgRomSizeKB = 16;                          // PRG ROM size in KB
    prgRomCount = gamerom[0x04];                // PRG ROM count in 16KB units
    if (prgRomCount != 0) {
        int size = prgRomSizeKB * 1024;         // PRG ROM size in Bytes
        prgRoms = (Uint8 **)SDL_malloc(sizeof(Uint8*) * prgRomCount);
        for(int i = 0; i < prgRomCount; i++) {
            prgRoms[i] = (Uint8*)SDL_malloc(sizeof(Uint8) * size);
            SDL_memcpy(prgRoms[i], &gamerom[addr], size);
            addr += size;
        }
        prgRomActive[0] = 0;                        // Start with first PRG ROM bank active for CPU read
        prgRomActive[1] = prgRomCount - 1;          // Last PRG ROM bank is often fixed at 0xC000-0xFFFF
    }
    
    // Extract CHR ROM information
    chrRomSizeKB = 8;                               // CHR ROM size in KB
    chrRomCount = gamerom[0x05];                    // CHR ROM count in 8KB units
    if (chrRomCount != 0) {
        int size = chrRomSizeKB * 1024;             // CHR ROM size in Bytes
        chrRoms = (Uint8 **)SDL_malloc(sizeof(Uint8*) * chrRomCount);
        for(int i = 0; i < chrRomCount; i++) {
            chrRoms[i] = (Uint8*)SDL_malloc(sizeof(Uint8) * size);
            SDL_memcpy(chrRoms[i], &gamerom[addr], size);
            addr += size;
        }
    }
    
    // Extract PRG RAM information (if present)
    prgRamSizeKB = 8;                               // PRG RAM size in KB
    prgRamCount = gamerom[0x08] & 0x0F;             // PRG RAM count in 8KB units (lower 4 bits of byte 0x08)
    if (prgRamCount != 0) {
        int size = prgRamSizeKB * 1024;             // PRG RAM size in Bytes
        prgRams = (Uint8 **)SDL_malloc(sizeof(Uint8*) * prgRamCount);
        for(int i = 0; i < prgRamCount; i++) {
            prgRams[i] = (Uint8*)SDL_malloc(sizeof(Uint8) * size);
            SDL_memcpy(prgRams[i], &gamerom[addr], size);
            addr += size;
        }
    }
}

Uint8* NESDSK::CHR2RGBA(int chrRomIndex, int offset) {
    // Implementation for converting CHR ROM pixel data to RGBA format for rendering
    Uint8 *chrRom = chrRoms[chrRomIndex];
    Uint8 *rgbaBuffer = (Uint8 *)SDL_malloc(128 * 128 * 4);       // 128 width x 128 height x 4 bytes per pixel for RGBA

    int src = offset * 4 * 1024; // Each CHR ROM bank is 32KB (256 tiles * 16 bytes per tile)

    for (int tileIndex = 0; tileIndex < 256; ++tileIndex) {
        // Calculate where this tile sits in the 2D grid layout
        int tileX = tileIndex % 16;
        int tileY = tileIndex / 16;
        
        int tileOffset = tileIndex * 16;
        
        // Loop through the 8 rows of pixels in the tile
        for (int y = 0; y < 8; ++y) {
            uint8_t lowByte  = chrRom[src + tileOffset + y];
            uint8_t highByte = chrRom[src + tileOffset + y + 8];
            
            // Loop through the 8 pixels in this row (left to right)
            for (int x = 0; x < 8; ++x) {
                // Pixel 0 is the Most Significant Bit (MSB), pixel 7 is the LSB
                int bitShift = 7 - x;
                
                uint8_t bit0 = (lowByte >> bitShift) & 0x01;
                uint8_t bit1 = (highByte >> bitShift) & 0x01;
                
                // Combine planes to find the 2-bit color palette index
                int colorIndex = (bit1 << 1) | bit0;
                Uint8 color = colorIndex * 85; // Scale 0-3 to 0-255 for grayscale (placeholder palette)
                
                // Map local tile coordinates to global texture space
                int pixelX = (tileX * 8) + x;
                int pixelY = (tileY * 8) + y;
                
                // Flatten the 2D coordinate into the 1D RGBA buffer index
                int targetIndex = (pixelY * 128 + pixelX) * 4;
                
                rgbaBuffer[targetIndex]     = color;
                rgbaBuffer[targetIndex + 1] = color;
                rgbaBuffer[targetIndex + 2] = color;
                rgbaBuffer[targetIndex + 3] = 0xFF;     // Alpha channel (fully opaque)
            }
        }
    }
    
    return rgbaBuffer;
}