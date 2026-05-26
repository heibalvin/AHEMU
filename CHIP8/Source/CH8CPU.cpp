#include "CH8CPU.hpp"
#include "CH8EMU.hpp" 

CH8CPU::CH8CPU(CH8EMU* parentEmu) : CH8COM(parentEmu) {
    buildOpcodesTable();
    reset();
}

void CH8CPU::powerOn() { 
    reset(); 
}

void CH8CPU::powerOff() { 
    reset(); 
}

void CH8CPU::reset() {
    I = 0; 
    PC = 0x200; 
    SP = 0; 
    DELAY_TIMER = 0; 
    SOUND_TIMER = 0; 
    opc = 0;
    opcode = nullptr;
    
    SDL_memset(V, 0, sizeof(V));
    SDL_memset(STACK, 0, sizeof(STACK));
}

void CH8CPU::step() {
    fetch();
    if (decode()) {
        execute();
    }
}

void CH8CPU::fetch() {
    // 1. Core Byte Assembly Extraction
    Uint8 highByte = readBus(PC);
    Uint8 lowByte  = readBus(PC + 1);
    opc            = (highByte << 8) | lowByte;
    
    // 2. Opcode Matrix Lookup Identification moved here
    opcode = nullptr; 
    for (int i = 0; i < TABLE_SIZE; ++i) {
        if ((opc & opcodes[i].mask) == opcodes[i].id) {
            opcode = &opcodes[i];
            break;
        }
    }
}

bool CH8CPU::decode() {
    // Error checking handles the identification fallout safely
    if (opcode == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Decoding Error: Unrecognized instruction signature parsed at address 0x%04X -> 0x%04X", PC, opc);
        return false;
    }

#ifdef DEBUG_MODE    
    // --- Format part 1: Registers Hexdump Space ---
    char regLog[256];
    SDL_snprintf(regLog, sizeof(regLog), 
        "V: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X SP: %02X I: %04X DT: %02X ST: %02X",
        V[0], V[1], V[2], V[3], V[4], V[5], V[6], V[7],
        V[8], V[9], V[10], V[11], V[12], V[13], V[14], V[15],
        SP, I, DELAY_TIMER, SOUND_TIMER);

    // --- Format part 2: Instruction Hexdump Format ---
    char instLog[64];
    SDL_snprintf(instLog, sizeof(instLog), "PC: %04X : %02X %02X", PC, (Uint8)(opc >> 8), (Uint8)(opc & 0x00FF));

    // --- Format part 3: Assembler Code Format interpretation ---
    char asmLog[64];
    Uint8 x   = (opc & 0x0F00) >> 8;
    Uint8 y   = (opc & 0x00F0) >> 4;
    Uint8 kk  = (opc & 0x00FF);
    Uint16 nnn = (opc & 0x0FFF);
    Uint8 n   = (opc & 0x000F);

    if (SDL_strcmp(opcode->mnemonic, "CLS") == 0 || SDL_strcmp(opcode->mnemonic, "RET") == 0) {
        SDL_snprintf(asmLog, sizeof(asmLog), "%s", opcode->mnemonic);
    } else if (SDL_strcmp(opcode->mnemonic, "JP addr") == 0 || SDL_strcmp(opcode->mnemonic, "CALL addr") == 0 || SDL_strcmp(opcode->mnemonic, "LD I, addr") == 0) {
        SDL_snprintf(asmLog, sizeof(asmLog), "%s 0x%03X", opcode->mnemonic, nnn);
    } else if (SDL_strcmp(opcode->mnemonic, "SE Vx, byte") == 0 || SDL_strcmp(opcode->mnemonic, "SNE Vx, byte") == 0 || SDL_strcmp(opcode->mnemonic, "LD Vx, byte") == 0 || SDL_strcmp(opcode->mnemonic, "ADD Vx, byte") == 0 || SDL_strcmp(opcode->mnemonic, "RND Vx, byte") == 0) {
        SDL_snprintf(asmLog, sizeof(asmLog), "%s V%X, 0x%02X", opcode->mnemonic, x, kk);
    } else if (SDL_strcmp(opcode->mnemonic, "DRW Vx, Vy") == 0) {
        SDL_snprintf(asmLog, sizeof(asmLog), "DRW V%X, V%X, %d", x, y, n);
    } else if (SDL_strcmp(opcode->mnemonic, "JP V0, addr") == 0) {
        SDL_snprintf(asmLog, sizeof(asmLog), "JP V0, 0x%03X", nnn);
    } else {
        SDL_snprintf(asmLog, sizeof(asmLog), "%s V%X, V%X", opcode->mnemonic, x, y);
    }

    SDL_Log("%s | %s | %s", regLog, instLog, asmLog);
#endif

    return true;
}

void CH8CPU::execute() {
    PC += 2; // Incremented explicitly here prior to underlying logic branching
    (this->*opcode->handler)();
}

// --- Hardware Opcode Core Logic Sets ───
void CH8CPU::opCLS()  { emu->ppu.clearScreen(); }
void CH8CPU::opRET()  { if (SP > 0) { SP--; PC = STACK[SP]; } }
void CH8CPU::opJP()   { PC = (opc & 0x0FFF); }
void CH8CPU::opCALL() { if (SP < 16) { STACK[SP] = PC; SP++; PC = (opc & 0x0FFF); } }

void CH8CPU::opSEByte()  { if (V[(opc & 0x0F00) >> 8] == (opc & 0x00FF)) PC += 2; }
void CH8CPU::opSNEByte() { if (V[(opc & 0x0F00) >> 8] != (opc & 0x00FF)) PC += 2; }
void CH8CPU::opSEReg()   { if (V[(opc & 0x0F00) >> 8] == V[(opc & 0x00F0) >> 4]) PC += 2; }
void CH8CPU::opLDByte()  { V[(opc & 0x0F00) >> 8] = (opc & 0x00FF); }
void CH8CPU::opADDByte() { V[(opc & 0x0F00) >> 8] += (opc & 0x00FF); }

void CH8CPU::opArithmetic() {
    Uint8 x = (opc & 0x0F00) >> 8;
    Uint8 y = (opc & 0x00F0) >> 4;
    switch (opc & 0x000F) {
        case 0x0: V[x] = V[y]; break;
        case 0x1: V[x] |= V[y]; V[0xF] = 0; break;
        case 0x2: V[x] &= V[y]; V[0xF] = 0; break;
        case 0x3: V[x] ^= V[y]; V[0xF] = 0; break;
        case 0x4: { Uint16 s = V[x] + V[y]; V[0xF] = (s > 255) ? 1 : 0; V[x] = s & 0xFF; break; }
        case 0x5: V[0xF] = (V[x] >= V[y]) ? 1 : 0; V[x] -= V[y]; break;
        case 0x6: V[0xF] = V[x] & 0x1; V[x] >>= 1; break;
        case 0x7: V[0xF] = (V[y] >= V[x]) ? 1 : 0; V[x] = V[y] - V[x]; break;
        case 0xE: V[0xF] = (V[x] & 0x80) >> 7; V[x] <<= 1; break;
    }
}

void CH8CPU::opSNEReg() { if (V[(opc & 0x0F00) >> 8] != V[(opc & 0x00F0) >> 4]) PC += 2; }
void CH8CPU::opLDI()   { I = (opc & 0x0FFF); }
void CH8CPU::opJPV0() { PC = (opc & 0x0FFF) + V[0]; }
void CH8CPU::opRND()  { V[(opc & 0x0F00) >> 8] = (0x42 & (opc & 0x00FF)); }

void CH8CPU::opDRW() {
    Uint8 x = (opc & 0x0F00) >> 8;
    Uint8 y = (opc & 0x00F0) >> 4;
    Uint8 h = (opc & 0x000F);
    V[0xF] = emu->ppu.writeSprite(V[x], V[y], &emu->bus.RAM[I], h);
}

void CH8CPU::opKeyboard() {
    Uint8 x = (opc & 0x0F00) >> 8;
    Uint8 sub = (opc & 0x00FF);
    if (sub == 0x9E)      { if (emu->con.isKeyPressed(V[x])) PC += 2; }
    else if (sub == 0xA1) { if (!emu->con.isKeyPressed(V[x])) PC += 2; }
}

void CH8CPU::opTimersAndMemory() {
    Uint8 x = (opc & 0x0F00) >> 8;
    switch (opc & 0x00FF) {
        case 0x07: V[x] = DELAY_TIMER; break;
        case 0x15: DELAY_TIMER = V[x]; break;
        case 0x18: SOUND_TIMER = V[x]; break;
        case 0x1E: I += V[x]; break;
        case 0x0A: {
            bool press = false;
            for (Uint8 i = 0; i < 16; ++i) {
                if (emu->con.isKeyPressed(i)) { V[x] = i; press = true; break; }
            }
            if (!press) PC -= 2; 
            break;
        }
        case 0x29: I = V[x] * 5; break;
        case 0x33: writeBus(I, V[x]/100); writeBus(I+1, (V[x]/10)%10); writeBus(I+2, V[x]%10); break;
        
        case 0x55: 
            for (int i = 0; i <= x; ++i) {
                writeBus(I + i, V[i]);
            }
            I += x + 1;
            break;
            
        case 0x65: 
            for (int i = 0; i <= x; ++i) {
                V[i] = readBus(I + i);
            }
            I += x + 1;
            break;
    }
}

void CH8CPU::opInvalid() {
    SDL_Log("Processor Fault: Encountered structural invalid execution token pattern: 0x%04X", opc);
}

void CH8CPU::buildOpcodesTable() {
    opcodes[0]  = { "CLS",        0xFFFF, 0x00E0, &CH8CPU::opCLS,             2, 1 };
    opcodes[1]  = { "RET",        0xFFFF, 0x00EE, &CH8CPU::opRET,             2, 1 };
    opcodes[2]  = { "JP addr",    0xF000, 0x1000, &CH8CPU::opJP,              2, 1 };
    opcodes[3]  = { "CALL addr",  0xF000, 0x2000, &CH8CPU::opCALL,            2, 1 };
    opcodes[4]  = { "SE Vx, byte",0xF000, 0x3000, &CH8CPU::opSEByte,          2, 1 };
    opcodes[5]  = { "SNE Vx, byte",0xF000, 0x4000, &CH8CPU::opSNEByte,         2, 1 };
    opcodes[6]  = { "SE Vx, Vy",  0xF00F, 0x5000, &CH8CPU::opSEReg,           2, 1 };
    opcodes[7]  = { "LD Vx, byte",0xF000, 0x6000, &CH8CPU::opLDByte,          2, 1 };
    opcodes[8]  = { "ADD Vx, byte",0xF000, 0x7000, &CH8CPU::opADDByte,         2, 1 };
    opcodes[9]  = { "ALU Vx, Vy", 0xF000, 0x8000, &CH8CPU::opArithmetic,      2, 1 };
    opcodes[10] = { "SNE Vx, Vy", 0xF00F, 0x9000, &CH8CPU::opSNEReg,          2, 1 };
    opcodes[11] = { "LD I, addr", 0xF000, 0xA000, &CH8CPU::opLDI,             2, 1 };
    opcodes[12] = { "JP V0, addr",0xF000, 0xB000, &CH8CPU::opJPV0,            2, 1 };
    opcodes[13] = { "RND Vx, byte",0xF000, 0xC000, &CH8CPU::opRND,             2, 1 };
    opcodes[14] = { "DRW Vx, Vy", 0xF000, 0xD000, &CH8CPU::opDRW,             2, 1 };
    opcodes[15] = { "KEYPROG",    0xF000, 0xE000, &CH8CPU::opKeyboard,        2, 1 };
    opcodes[16] = { "TIMEMEM",    0xF000, 0xF000, &CH8CPU::opTimersAndMemory,  2, 1 };
}